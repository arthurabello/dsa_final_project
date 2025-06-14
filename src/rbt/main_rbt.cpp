#include <iostream> 
#include <string>   

#include "../data.cpp"
#include "rbt.cpp"
#include "rbt.h"


int main(int argc, char *argv[]){

    // Verifying if there is correctly 4 arguments
    if(argc!=4){
        std::cerr << "Using: ./rbt <search|stats> <n_docs> <diretorio>" << std::endl;
        return 1; //error code
    } 

    // Getting args
    std::string command = argv[1];
    std::string n_docs_str = argv[2];
    std::string dir_path = argv[3];

    // Converting n_docs_str to int
    int n_docs_int;
    try{
        n_docs_int = std::stoi(n_docs_str); // string to int
        if(n_docs_int<=0){
            std::cerr << "Error <n_docs> invalid. It must be positive" << std::endl;
            return 1;
        }
    } catch (const std::invalid_argument& ia){
        std::cerr << "Error <n_docs> invalid. It must be a positive integer" << std::endl;
        return 1;
    } catch (const std::out_of_range& oot){
        std::cerr << "Error <n_docs> invalid. It is out of range" << std::endl;
        return 1;
    }

    // Printing infos
    std::cout << "Command: " << command << ", Number of documents: " << n_docs_int << ", Directory: " << std::endl;

    // Initializing bst tree and stats struct
    TREE::BinaryTree* rbt_tree = TREE::createTree();
    TREE::AggregateStats rbt_stats;
    rbt_stats.tree_type = "RBT";

    // Starting overall total time
    auto overall_indexing_start_time = std::chrono::high_resolution_clock::now() // this auto identifies this super specific type 

    // Get List of .txt files
    std::vector<std::string> all_filenames = DATA::list_txt_files_in_path(dir_path);
    if (all_filenames.empty()) { 
        /* ... handle empty or error ... */ 
    }

    // Seeing if there is enough files to process n_docs_int
    int files_to_process_count;
    if(all_filenames.size() <= n_docs_int){
        files_to_process_count = all_filenames.size();
        std::cout << "Processing the first " << files_to_process_count << " docs. There were only " << files_to_process_count << " files available." << std::endl;
    }else{
        files_to_process_count = n_docs_int;
        std::cout << "Processing the first " << files_to_process_count << " docs." << std::endl;
    }

    // Updating the count of files to process in stats
    bst_stats.num_docs_indexed = files_to_process_count;

    // Loop through selected files for indexing
    for (int i = 0; i < files_to_process_count; ++i) {

        // Making logic to access file (path + its name = whole path)
        std::string& filename_only = all_filenames[i];
        std::string full_path_to_file = dir_path;
        if(!dir_path.empty() && dir_path.back() != '/' && dir_path.back() != '\\'){ // add a "universal" bar at the end in case it's not there
            full_path_to_file += "/";
        }
        full_path_to_file += filename_only;

        // Getting the document id (to put in inverse indexing)
        int document_id; // default for error is -1
        try {
            // Find the position of '.' in "22.txt" for ex
            size_t dot_position = filename_only.find('.'); // size_t is a positive integer
            if (dot_position != std::string::npos) { // npos indicates it was not found
                // Extract the substring before '.', which is "22"
                std::string id_str = filename_only.substr(0, dot_position);
                // Convert this string "22" to an integer
                document_id = std::stoi(id_str);
            } else {
                std::cerr << "Warning: Filename '" << filename_only
                          << "' does not contain a '.' to separate ID from extension. Using default ID -1." << std::endl;
                document_id = -1;
            }
        } catch (const std::invalid_argument& ia) {
            std::cerr << "Warning: Could not parse numeric ID from filename part '"
                      << filename_only.substr(0, filename_only.find('.')) << "' for file '" << filename_only << std::endl;
            document_id = -1; 
        } catch (const std::out_of_range& oor) {
            std::cerr << "Warning: Numeric ID from filename part '"
                      << filename_only.substr(0, filename_only.find('.')) << "' for file '" << filename_only
                      << "' is out of range for an integer. " << std::endl;
            document_id = -1;
        }

        // Getting a vector of tokens out of content read
        std::vector<std::string> words_in_doc = DATA::tokenize(full_path_to_file);
        // Just checking again (tokenize also does a check)
        if (words_in_doc.empty() && !std::ifstream(full_path_to_file).good()) {
             std::cerr << "Warning: No words tokenized or error reading file: " << full_path_to_file << std::endl;
             continue;
        }

        // Iter in each token of current file being analized
        for(const std::string& word : words_in_doc){
            // Finally insert word in tree and update stats qith its stats
            if(!word.empty()){
                rbt_stats.total_words_processed ++;

                TREE::InsertResult result = TREE::RBT::insert(rbt_tree, word, document_id);

                rbt_stats.sum_of_insertion_times_ms += result.executionTime;
                rbt_stats.total_comparisons_insertion += result.numComparisons;
            }
        }
    }

    // getting final indexing time for stats
    auto overal_indexing_end_time = std::chrono::high_resolution_clock::now();
    rbt_stats.total_indexing_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(overal_indexing_end_time - overall_indexing_start_time).count()
    std::cout << "Indexing completed in " << rbt_stats.total_indexing_time_ms << " ms." << std::endl;

    // updating stats nodes and height
    rbt_stats.final_node_count = TREE::countNodes(rbt_tree);
    rbt_stats.final_tree_height = TREE::calculateHeight(rbt_tree);

    // Running search in stats or search only
    if(command == "stats"){
        std::cout << "Documents indexed: " << rbt_stats.num_docs_indexed << std::endl;
        std::cout << "etc: " << rbt_stats. << std::endl;
        std::cout << "etc: " << rbt_stats. << std::endl;

        // TODO
        save_stats_to_csv();

    } else if (command == "search"){

        std::string query_from_term;
        while(true){
            std::cout << "Insert word to search or 'exit_search' to quit: ";
            std::cin >> query_from_term; // read word from terminal

            if(query_from_term == "exit_search"){
                break;
            }
            // Normalizing word from query
            std::string normalised_query = DATA::normalise(query_from_term);
            if(normalised_query.empty()){
                std::cout << "Invalid search after normalization: " << normalised_query << " Try again." << std::endl;
                continue;
            }
            std::cout << "Searching for normalized word: '" << normalised_query << "'..." << std::endl;

            // Call the generic search function from tree_utils
            TREE::SearchResult search_result = TREE::search(rbt_tree, normalised_query);

            // Display search results
            std::cout << "  Search Time: " << search_result.executionTime << " ms" << std::endl;
            std::cout << "  Comparisons: " << search_result.numComparisons << std::endl;
            if(search_result.found){
                std::cout << "Word " << normalised_query << " found in documents IDs: " << std::endl;
                for(size_t k = 0; k < search_result.documentIds.size(); k++){
                    std::cout << search_result.documentIds[k] << "; ";
                }
                std::cout << std::endl;
            }else{
                std::cout << "Word " << normalised_query << " not found." << std::endl;
            }
            
        }

    } else{
        std::cerr << "Error: Unkowned command: " << command << ". Use 'search' or 'stats'." << std::endl;
        return 1;
    }
    
    // Cleanup: Deallocate the tree
    std::cout << "Deallocating tree..." << std::endl;
    TREE::destroy(rbt_tree); 
    rbt_tree = nullptr;

    return 0;
}
