#pragma once
// Implementation file for MD_Menu library
//
// See the main header file MD_Menu.h for more information

/**
\page pageMenu Menu System Concepts
Menu Systems
============
A menu is a list of options from which the user makes a selection. Each
option is a brief description for a selection (menu label) that can lead
to a sub-menu or a specific action being performed.

The tree data structure is one of the most obvious structures used to represent
menus. In trees information naturally forms a hierarchy - menu entries are
naturally ordered *above* or *below* other entries. The tree data structure
is a very efficient way to represent this type of information. Although a tree
has built-in hierarchical structure, it can be stored as arrays of data.

Tree Structure Primer
---------------------
This structure is named a *tree structure* because the classic representation
resembles a tree, even though the chart is generally upside down compared
to an actual tree, with the *root* at the top and the *leaves* at the
bottom. The tree elements are called *nodes*. The lines connecting nodes
are called *branches*. Nodes with no sub-nodes are called leaf nodes, end
nodes, terminal nodes or leaves.

![A Tree Structure] (Tree_Structure.png "Tree Structure")

A tree is a special kind of graph that follows a particular set of rules
and definitions:
- **Connected** - A graph can be a tree if it is connected. Each node is
connected with a link to at least one other node.
- **Acyclic** - A graph can be a tree if is acyclic (ie, it has no cycles
or loops in it).
That means there's only one route from any node to any other node. This also
means that a node may have many children but only one parent.
- **Root** - The term root commonly refers to a top-most node (ie, the one
with no parent). In the figure above, node F is the root of the tree. The
root is the starting for traversing any tree structure.
- **Descendant** - A descendant is a node that is farther away from the root
than some other node. The term descendant is always referenced from another
node. In the example, nodes I and H are descendants of node G.
- **Parent** - Parent is considered the node that is closer to the root node
by one link or vertex, in the same branch. In the figure, node B is the parent
of nodes A and D. The ultimate parent in a tree is the root node.
- **Sibling** - Sibling (brother or sister) nodes share the same parent. In
the example, nodes A and D are siblings.
- **Ancestor** - An ancestor is any node between a given node and the root,
including the root. In the figure, the ancestors of node H are nodes I, G,
and F.
- **Leaf** or **Terminal Node** - A node is terminal if it has no children. In
the example, node C is a leaf.
- **Height** or **Depth** - The height of a tree is defined as the number of vertices
traversed to get to the most distant node. In the figure, the height of the
tree is equal to three.
*/

/**
 * \file
 * \brief Contains internal library definitions
 */

#define MD_DEBUG 0  ///< set to 1 to enable library debugging mode

#if MD_DEBUG
#define MD_PRINTS(s)   { Serial.print(F(s)); }                  ///< Library debugging output macro
#define MD_PRINT(s, v) { Serial.print(F(s)); Serial.print(v); } ///< Library debugging output macro
#define MD_PRINTX(s, v) { Serial.print(F(s)); Serial.print(F("0x")); Serial.print(v); } ///< Library debugging output macro
#else
#define MD_PRINTS(s)      ///< Library debugging output macro
#define MD_PRINT(s, v)    ///< Library debugging output macro
#define MD_PRINTX(s, v)   ///< Library debugging output macro
#endif

const char FLD_PROMPT[] = ":";   ///< Prompt separator between input field label and left delimiter
const char FLD_DELIM_L[] = "[";  ///< Left delimiter for variable field input
const char FLD_DELIM_R[] = "]";  ///< Right delimiter for variable field input
const char MNU_DELIM_L[] = "<";  ///< Left delimiter for menu option label
const char MNU_DELIM_R[] = ">";  ///< Right delimiter for menu option label

const char INP_BOOL_T[] = "Y";   ///< Boolean input True display value. Length should be same as INP_BOOL_F
const char INP_BOOL_F[] = "N";   ///< Boolean input False display value. Length should be same as INP_BOOL_T

const char INP_NUMERIC_OFLOW = '#';  ///< Indicator for numeric overflow
const char LIST_SEPARATOR = '|';     ///< Separator character for list items
const char DECIMAL_POINT = '.';      ///< Decimal point character for floating point input
const char FLOAT_DECIMALS = 2;       ///< Number of float decimals implied in uint32_t value
const char ENGU_DECIMALS = 3;        ///< Number of engineering units decimals implied in uint32_t value
const uint8_t ENGU_RANGE = 18;       ///< Syymentrical range of power prefixes from 10^-ENGU_RANGE to 10^+ENGU_RANGE

#define INP_PRE_SIZE(mi)  (strlen(mi->label) + strlen(FLD_PROMPT) + strlen(FLD_DELIM_L))  ///< Size of text pre variable display
#define INP_POST_SIZE(mi) (strlen(FLD_DELIM_R))  ///< Size of text after variable display

// Global options and flags management
#define SET_FLAG(f)   { _options |= (1<<f);  MD_PRINTX("\nSet Flag ",_options); }  ///< Set a flag
#define CLEAR_FLAG(f) { _options &= ~(1<<f); MD_PRINTX("\nClr Flag ", _options); } ///< Reset a flag
#define TEST_FLAG(f)  ((_options>>f) & 1)  ///< Test a flag 

#define F_INMENU 0    ///< Flag currently running a menu
#define F_INEDIT 1    ///< Flag currently editing a value
#define F_MENUWRAP 2  ///< Flag to wrap around ends of menu and list selections
#define F_AUTOSTART 3 ///< Flag auto start the menu system opn SEL

