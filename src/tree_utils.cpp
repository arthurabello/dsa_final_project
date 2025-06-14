#include "tree_utils.h"
#include <vector>
#include <chrono>
#include <cstring>
#include <fstream>

namespace TREE{
	// Sets for 0 if it the tree doesnt support red-black
    Node* createNode(std::string word, std::vector<int>documentIds, int color) {

        Node* newNode = new Node;
        newNode->word = word;
        newNode->documentIds = documentIds;
        newNode->parent = nullptr;
        newNode->left = nullptr;
        newNode->right = nullptr;
        newNode->height = 0; //height of a new node is 0
        newNode->isRed = color; //0 for red, 1 for black
        return newNode;
    }

    BinaryTree* createTree(){
        BinaryTree* newBinaryTree = new BinaryTree{nullptr};
        return newBinaryTree;
    }

    SearchResult search(BinaryTree* binary_tree, const std::string& word) {
        auto start_time = std::chrono::high_resolution_clock::now(); //start measuring time

        if (binary_tree == nullptr || binary_tree->root == nullptr) {
            auto end_time = std::chrono::high_resolution_clock::now(); //done lol
            double duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0;
            return {0, {}, duration, 0};

        } else {
            Node* current_node = binary_tree->root;
            int number_of_comparisons = 0;

            while (current_node != nullptr) {
                number_of_comparisons++;

                int compareResult = strcmp(word.c_str(), current_node->word.c_str());

                if (compareResult == 0) { //found!
                    auto end_time = std::chrono::high_resolution_clock::now();
                    double duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0;
                    return {1, current_node->documentIds, duration, number_of_comparisons};

                } else if (compareResult < 0) {
                    current_node = current_node->left; //go left because word is smaller
                } else {
                    current_node = current_node->right; //go right because word is bigger
                }
            }

            //if word not found
            auto end_time = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0;
            return {0, {}, duration, number_of_comparisons};
        }
    }

    void deletionPostOrder(Node* n){
        if(n != nullptr){
            deletionPostOrder(n->left);
            deletionPostOrder(n->right);
            delete n;
        }
    }
    
    void destroy(BinaryTree* binary_tree){
        Node* root = binary_tree->root;
		    deletionPostOrder(root);
		    delete binary_tree;
    }

    int calculateHeight(Node* root){
        //Treats the case in which the root is empty.
        if(root == nullptr) return -1; //I fixed this
        
        //Copies the left subtree.
        Node* leftNode = root->left;

        //Copies the right subtree.
        Node* rightNode = root->right;

        //Calculate the height of the tree by a recursive call.
        int height = 1 + std::max(calculateHeight(rightNode),calculateHeight(leftNode));

        return height;
    }

    void updateHeightUp(Node* node) {
        if (node == nullptr) {
            return;
        }
		
		int originalHeight = node->height;
		
        // Calculates the heights of the children and get the max
        int leftHeight;
        if (node->left == nullptr) {
            leftHeight = -1;
        } else {
            leftHeight = node->left->height;
        }

        int rightHeight;
        if (node->right == nullptr) {
            rightHeight = -1;
        } else {
            rightHeight = node->right->height;
        }
		
        int maxHeight = std::max(leftHeight,rightHeight);

        int newHeight = 1 + maxHeight;

        node->height = newHeight;
		
        if (originalHeight == newHeight && (node->right != nullptr || node->left != nullptr)) {
            return;
        }
		
        // Calculates the height of the father by a recursive call
        updateHeightUp(node->parent);

    }

    void save_stats_to_csv(const AggregateStats& stats, const std::string& filename) {
        std::ofstream file(filename);

        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << " for writing.\n";
            return;
        }

        // Header
        file << "tree_type,num_docs_indexed,"
            << "total_indexing_time_ms,total_words_processed,total_comparisons_insertion,sum_of_insertion_times_ms,max_insertion_time_ms,"
            << "total_search_time_ms,total_searches,total_comparisons_search,sum_of_search_times_ms,max_search_time_ms,"
            << "final_node_count,final_tree_height,final_tree_min_depth,relative_balance,balance_difference,"
            << "average_insertion_time_ms,average_comparisons_insertion,average_search_time_ms,average_comparisons_search\n";

        // Data
        file << stats.tree_type << ","
            << stats.num_docs_indexed << ","
            << stats.total_indexing_time_ms << ","
            << stats.total_words_processed << ","
            << stats.total_comparisons_insertion << ","
            << stats.sum_of_insertion_times_ms << ","
            << stats.max_insertion_time_ms << ","
            << stats.total_search_time_ms << ","
            << stats.total_searches << ","
            << stats.total_comparisons_search << ","
            << stats.sum_of_search_times_ms << ","
            << stats.max_search_time_ms << ","
            << stats.final_node_count << ","
            << stats.final_tree_height << ","
            << stats.final_tree_min_depth << ","
            << stats.relative_balance << ","
            << stats.balance_difference << ","
            << stats.average_insertion_time_ms << ","
            << stats.average_comparisons_insertion << ","
            << stats.average_search_time_ms << ","
            << stats.average_comparisons_search
            << "\n";

        file.close();
        std::cout << "Statistics saved to: " << filename << std::endl;
    }

    int calculateMinDepth(Node* root) {
        if (root == nullptr) {
            return 0;
        }

        // If is a leaf
        if (root->left == nullptr && root->right == nullptr) {
            return 0;
        }

        // Recurse only on the right subtree
        if (root->left == nullptr) {
            return 1 + calculateMinDepth(root->right);
        }

        // Recurse only on the left subtree
        if (root->right == nullptr) {
            return 1 + calculateMinDepth(root->left);
        }

        // Smaller depth between left and right
        int leftDepth = calculateMinDepth(root->left);
        int rightDepth = calculateMinDepth(root->right);

        return 1 + std::min(leftDepth, rightDepth);
    }

    int countNodes(Node* root) {
        if (root == nullptr) {
            return 0;
        }

        return 1 + countNodes(root->left) + countNodes(root->right);
    }

    double getAverageInsertionTime(const AggregateStats& stats) {
        if (stats.total_words_processed == 0){
            return 0.0;
        }

        return stats.sum_of_insertion_times_ms / stats.total_words_processed;
    }

    double getAverageComparisonsPerInsertion(const AggregateStats& stats) {
        if (stats.total_words_processed == 0) {
            return 0.0;
        }

        return static_cast<double>(stats.total_comparisons_insertion) / stats.total_words_processed;
    }

    double getAverageSearchTime(const AggregateStats& stats) {
        if (stats.total_searches == 0) {
            return 0.0;
        }

        return stats.sum_of_search_times_ms / stats.total_searches;
    }

    double getAverageComparisonsPerSearch(const AggregateStats& stats) {
        if (stats.total_searches == 0){
            return 0.0;
        }

        return static_cast<double>(stats.total_comparisons_search) / stats.total_searches;
    }

    double getRelativeBalance(const AggregateStats& stats) {
        if (stats.final_tree_min_depth == 0) {
            return 0.0;
        }

        return static_cast<double>(stats.final_tree_height) / stats.final_tree_min_depth;
    }

    int getBalanceDifference(const AggregateStats& stats) {
        return stats.final_tree_height - stats.final_tree_min_depth;
    }

    void updateAllAggregateStats(AggregateStats& stats, BinaryTree* tree) {
        stats.final_node_count = countNodes(tree->root);
        stats.final_tree_height = calculateHeight(tree->root);
        stats.final_tree_min_depth = calculateMinDepth(tree->root);
        stats.average_insertion_time_ms = getAverageInsertionTime(stats);
        stats.average_comparisons_insertion = getAverageComparisonsPerInsertion(stats);
        stats.average_search_time_ms = getAverageSearchTime(stats);
        stats.average_comparisons_search = getAverageComparisonsPerSearch(stats);
        stats.relative_balance = getRelativeBalance(stats);
        stats.balance_difference = getBalanceDifference(stats);

    }
	
	
	// void isBalanced(Node* root) {
		// if (std::abs(AVL::bf(root))<2) {
			// isBalanced(root->left);
			// isBalanced(root->right);
		// }
		// std::cout << "Árvore desbalanceada na palavra: "<< root->word<<"\n";
	// }
	
}

