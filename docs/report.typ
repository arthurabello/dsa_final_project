#import "@preview/ctheorems:1.1.3": *
#import "@preview/plotst:0.2.0": *
#import "@preview/codly:1.2.0": *
#import "@preview/codly-languages:0.1.1": *
#codly(languages: codly-languages)

#show: codly-init.with()
#show: thmrules.with(qed-symbol: $square$)  
#show link: underline
#show ref: underline

#set heading(numbering: "1.1.")
#set page(numbering: "1")
#set heading(numbering: "1.")
#set math.equation(
  numbering: "1",
  supplement: none,
)
#show ref: it => {
  // provide custom reference for equations
  if it.element != none and it.element.func() == math.equation {
    // optional: wrap inside link, so whole label is linked
    link(it.target)[eq.~(#it)]
  } else {
    it
  }
}

#let theorem = thmbox("theorem", "Theorem", fill: rgb("#ffeeee")) //theorem color
#let corollary = thmplain(
  "corollary",
  "Corollary",
  base: "theorem",
  titlefmt: strong
)
#let definition = thmbox("definition", "Definition", inset: (x: 1.2em, top: 1em))
#let example = thmplain("example", "Example").with(numbering: "1.")
#let proof = thmproof("proof", "Proof")

//shortcuts


#align(right, text(12pt)[
  *FGV/EMAp*\
])


#align(center, text(17pt)[
  *Final Project - Inverted Index and Comparative Analysis*
])

#align(center, text(11pt)[
  Arthur Rabello Oliveira #footnote[#link("https://emap.fgv.br/")[Escola de Matemática Aplicada, Fundação Getúlio Vargas (FGV/EMAp)], email: #link("mailto:arthur.oliveira.1@fgv.edu.br")], Gabrielle Mascarelo, Eliane Moreira, Nícolas Spaniol, Gabriel Carneiro
])
#set par(first-line-indent: 1.5em,justify: true)

#align(center)[
  *Abstract*\
  We present the implementation and comparative analysis of three fundamental data structures for indexing and searching textual documents: the Classic Binary Search Tree (BST), the AVL Tree, and the Red-Black Tree (RBT). Each structure was implemented with its core operations, including insertion and search. Unit tests were developed to validate the correctness and performance of these implementations. We also provide a further comprehensive comparative study of the three trees based on their time complexity, balancing efficiency, and suitability for document indexing. The results demonstrate the trade-offs between implementation complexity and query performance, offering insights into the practical considerations for choosing appropriate search tree structures in information retrieval systems.
]

#outline()

#pagebreak()

= Introduction
<section_introduction>

== Context
<section_context>

Humanity now produces more text in a single day than it did in the first two millennia of writing combined. Search engines must index billions of web pages, e-commerce sites hundreds of millions of product descriptions, and DevOps teams terabytes of log lines every hour. Scanning those datasets sequentially would be orders of magnitude too slow; instead, virtually all large-scale retrieval systems rely on an *inverted index*, a data structure that stores, for each distinct term, the identifiers of documents in which it appears.

== Problem Statement
<section_problem_statement>

While the logical view of an inverted index is a simple dictionary, its physical realisation must support two conflicting workloads:

- *Bulk ingestion* of millions of (term, docID) pairs per second.

- *Sub-millisecond* queries for ad-hoc keyword combinations.

Choosing the proper data structure is therefore a trade-off between build-time cost and implementation complexity.

== Objectives
<section_objectives>

+ *Implement* BST, AVL and Red-Black Tree insertion, deletion and search in C++.

+ *Build* an inverted index over a 10 k-document corpus with each tree.

+ *Measure* build time, query latency, and memory usage under identical workloads.

+ *Discuss* which structure best balances simplicity and performance for mid-scale information-retrieval tasks.

= Motivation
<section_motivation>

== Why Inverted Index?
<section_why_inverted_index>


#table(
  columns: 3,

  table.header(
    [*Domain*],
    [*Real-world system*],
    [*Role of the inverted index*],
  ),

  [Web search],            [Google, Bing, DuckDuckGo],          [Core term $->$ page mapping],
  [Enterprise search],     [Apache Lucene & Elasticsearch],     [Underlying index files and query engine],
  [Database systems],      [Postgres GIN & CockroachDB],        [Full-text and JSONB indexing],
  [Observability / Logs],  [Splunk, OpenObserve],               [Fast filtering / aggregation of terabyte-scale logs],
  [Bioinformatics],        [VariantStore, PAC],                 [Searching billions of DNA k-mers],
  [Operating systems],     [Linux schedulers & timers],         [Kernel subsystems use RBTs—conceptually an inverted index over time or PID keys],
)

These examples shows the ubiquity of inverted indexes in modern era. From web search engines to bioinformatics, inverted indexes are the backbone of efficient information retrieval systems.

= Implementations
<section_implementations>
== Binary Search Tree (BST)
<section_bst_implementation>

Since the AVL and RBT trees are both subsystems of the classic BST, we have implemented the classic BST in the #link("https://github.com/arthurabello/dsa-final-project/blob/main/src/tree_utils.cpp")[`tree_utils`] module, later used as based for the AVL and BST

=== Algorithms
<section_bst_algorithms>

#link("https://github.com/arthurabello/dsa-final-project/blob/main/src/tree_utils.cpp")[Here] are all the functions written for the classic BST, and #link("https://github.com/arthurabello/dsa-final-project/blob/main/src/tree_utils.h")[here] is the header file with the corresponding documentation. The list of functions is:

- `createNode`

- `createTree`

- `search`

- `deletionPostOrder`

- `destroy`

- `calculateHeight`

- `updateHeightUp`

=== Complexity Analysis
<section_bst_complexity_analysis>

Below is a full complexity analysis:

*`createNode`*:

Clearly $O(1)$, the function allocates memory for a new node and initializes it, independent of the size of the tree.

*`createTree`*:

Also $O(1)$, an empty BST is allocated and initialized with a `nullptr` root.

*`search`*:

The search operation, unavoidably, has a time complexity of $O(h)$, with $h$ being the height of the tree. The function follows a single root-to-leaf path in the BST, making at most $h$ comparisons. No recursion, only a few local variables.

*`deletionPostOrder`*:

This function is $O(phi)$, where $phi$ is the size of the subtree rooted at the node to be deleted. It is a classic post-order traversal: each node is visited & deleted precisely once.

*`destroy`*:

This functon simply calls `deletionPostOrder` on the root node, so it is $O(h)$.

*`calculateHeight`*:

This is $O(k)$, with $k = "subtree size"$. It 	recursively explores both children of every node once to compute `max(left,right)+1`.

*`updateHeightUp`*:

This function is $O(h)$. It iterates upward, recomputing height until it stops changing or reaches the root; at most $h$ ancestor steps, no recursion on children. 

== AVL Tree
<section_avl_implementation>
=== Algorithms
<section_avl_algorithms>

The functions implemented _stritly_ for the AVL can be found #link("https://github.com/arthurabello/dsa-final-project/blob/main/src/avl/avl.cpp")[here], and the header file with the corresponding documentation #link("https://github.com/arthurabello/dsa-final-project/blob/main/src/avl/avl.h")[here]. We have used many of the BST functions, as previously stated. The list of AVL-functions is:

- `getHeight(Node*)`

- `bf(Node*)` [balance factor]

- `leftRotation(BinaryTree&, Node*)`

- `rightRotation(BinaryTree&, Node*)`

- `insert(BinaryTree& binary_tree, const std::string& word, int documentId)`

- `remove(Node*&, key)`

- `remove(BinaryTree&, key)`

- `updateHeightUp(Node*)`

- `clear(BinaryTree*)`

- `printInOrder`

- `printLevelOrder`

=== Complexity Analysis
<section_avl_complexity_analysis>

Below is a full complexity analysis of the AVL functions:

*`getHeight`*:

$O(1)$, it simply returns the stored `height` of a node.

*`bf(Node*)`*:

Also $O(1)$, this is nothing more than a subtraction of two integers.

*`leftRotation(BinaryTree&, Node*)`*:

This is a constant quantity of pointer rewires and some height updates, so $O(1)$.

*`rightRotation(BinaryTree&, Node*)`*:

Same as above, $O(1)$.

*`insert(BinaryTree& binary_tree, const std::string& word, int documentId)`*:

This is $O(h)$, where $h$ is the height of the tree. It does one BST descent $O(h)$ at most one rebalance per level. The rotations are constant.

*`remove(Node*&, key)`*:

This is $O(h)$. It is a classic BST deletion + at most one structural deletion + up-to-root rebalancing.

*`remove(BinaryTree&, key)`*:

Same as above, $O(h)$.

*`updateHeightUp(Node*)`*:

This function iterates upward recomputing height until the value stabilises or the root is reached. Therefore it is $O(h)$.

*`clear(BinaryTree*)`*:

Clearly $O(n)$, where $n$ is the size of the tree. It removes each node once.

*`printInOrder`*:

This is a classic traversal visiting every node once, so it is $O(n)$.

*`printLevelOrder`*:

Same as above, $O(n)$.





== Red-Black Tree (RBT)
<section_rbt_impementation>

=== Algorithms
<section_rbt_algorithms>

=== Complexity Analysis
<section_rbt_complexity_analysis>

== Inverted Index
<section_inverted_index_implementation>

=== Algorithms
<section_inverted_index_algorithms>

=== Complexity Analysis
<section_inverted_index_complexity_analysis>

= Testing and Validation
<section_testing_validation>
== Unit Testing Method
<section_unit_testing_method>

We have used the #link("https://github.com/ThrowTheSwitch/Unity/tree/b9d897b5f38674248c86fb58342b87cb6006fe1f")[*Unity*] submodule for unit testing (espanio faz teu nome)

All trees were equivalently tested under the same principles blablabla

The testing module can be found here blablabla


= Comparative Analysis
<section_comparative_analysis>

== The Experiment
<section_the_experiment>

== Memory Usage
<section_memory_usage>

== Time Complexity
<section_time_complexity>

= Conclusion
<section_conclusion>

== Summary of Findings
<section_summary_findings>


= Source code
<section_source_code_repository>

All implementations and tests are available in the public repository at https://github.com/arthurabello/dsa-final-project

= Task Division  (Required by the professor)
<section_task_division>
== Arthur Rabello Oliveira
<section_task_division_arthur>

Contributed with:

- Keeping the repository civilized (main-protecting rules, enforcing code reviews)

- Writing and documenting the `Makefile` for building the project

- Writing the `README.md` and `report.typ`

- Writing and documenting functions for the classic BST in `bst.cpp`

== Gabrielle Mascarelo
<section_task_division_gabrielle>

Contributed with:

- Writing and documenting functions to read files in `data.cpp`

- Structuring statistics in the CLI 

== Eliane Moreira
<section_task_division_eliane>

Contributed with:

- Testing all function related to the BST

- Writing and documenting functions for `tree_utils.cpp`

- Fixing bugs in `data.h`

== Nícolas Spaniol
<section_task_division_nicolas>

Contributed with:

- Code reviews and suggestions for improvements in the codebase.

- Built from scratch the JavaScript visualization of all trees

== Gabriel Carneiro
<section_task_division_gabriel>

Contributed with:

- Writing and documenting functions for the classic BST

- Writing and documenting functions for the AVL Tree

- Testing functions for `tree_utils.cpp`

