#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SYMBOLS 26
#define MAX_RULES 50
#define MAX_RHS 20

// Structure to represent a production rule
typedef struct {
    char lhs;                   // Left-hand side nonterminal
    char rhs[MAX_RHS];          // Right-hand side of production
    int rhs_length;             // Length of right-hand side
} Production;

// Structure to represent the grammar
typedef struct {
    Production productions[MAX_RULES];
    int num_productions;
    bool first_sets[MAX_SYMBOLS][MAX_SYMBOLS];  // First sets for each nonterminal
    bool visited[MAX_SYMBOLS];                  // For cycle detection
} Grammar;

// Initialize grammar
void init_grammar(Grammar* g) {
    g->num_productions = 0;
    memset(g->first_sets, 0, sizeof(g->first_sets));
    memset(g->visited, 0, sizeof(g->visited));
}

// Add a production to the grammar
void add_production(Grammar* g, char lhs, const char* rhs) {
    Production* p = &g->productions[g->num_productions++];
    p->lhs = lhs;
    p->rhs_length = strlen(rhs);
    strcpy(p->rhs, rhs);
}

// Check if a symbol is a terminal (lowercase)
bool is_terminal(char symbol) {
    return symbol >= 'a' && symbol <= 'z';
}

// Check if a symbol is a nonterminal (uppercase)
bool is_nonterminal(char symbol) {
    return symbol >= 'A' && symbol <= 'Z';
}

// Convert symbol to index
int symbol_to_index(char symbol) {
    if (is_terminal(symbol)) return symbol - 'a';
    return symbol - 'A';
}

// Calculate First set for a nonterminal
void calculate_first(Grammar* g, char symbol) {
    int symbol_idx = symbol_to_index(symbol);
    
    // If already visited, return to avoid cycles
    if (g->visited[symbol_idx]) return;
    g->visited[symbol_idx] = true;
    
    // For each production with this nonterminal on the LHS
    for (int i = 0; i < g->num_productions; i++) {
        if (g->productions[i].lhs != symbol) continue;
        
        // Process the first symbol of RHS
        char first_symbol = g->productions[i].rhs[0];
        
        if (is_terminal(first_symbol)) {
            // If terminal, add to First set
            g->first_sets[symbol_idx][symbol_to_index(first_symbol)] = true;
        } else if (is_nonterminal(first_symbol)) {
            // If nonterminal, calculate its First set recursively
            calculate_first(g, first_symbol);
            
            // Add all symbols from First(first_symbol) to First(symbol)
            for (int j = 0; j < MAX_SYMBOLS; j++) {
                if (g->first_sets[symbol_to_index(first_symbol)][j]) {
                    g->first_sets[symbol_idx][j] = true;
                }
            }
        }
    }
}

// Calculate First sets for all nonterminals
void calculate_all_firsts(Grammar* g) {
    // Reset visited array
    memset(g->visited, 0, sizeof(g->visited));
    
    // Calculate First sets for all nonterminals
    for (int i = 0; i < g->num_productions; i++) {
        calculate_first(g, g->productions[i].lhs);
    }
}

// Print First sets
void print_first_sets(Grammar* g) {
    for (int i = 0; i < g->num_productions; i++) {
        char symbol = g->productions[i].lhs;
        int symbol_idx = symbol_to_index(symbol);
        
        // Only print each nonterminal once
        bool already_printed = false;
        for (int j = 0; j < i; j++) {
            if (g->productions[j].lhs == symbol) {
                already_printed = true;
                break;
            }
        }
        if (already_printed) continue;
        
        printf("First(%c) = { ", symbol);
        bool first = true;
        for (int j = 0; j < MAX_SYMBOLS; j++) {
            if (g->first_sets[symbol_idx][j]) {
                if (!first) printf(", ");
                printf("%c", 'a' + j);
                first = false;
            }
        }
        printf(" }\n");
    }
}

int main() {
    Grammar g;
    init_grammar(&g);
    
    add_production(&g, 'S', "aA");
    add_production(&g, 'A', "a");
    add_production(&g, 'A', "b");
    add_production(&g, 'A', "d");
    
    calculate_all_firsts(&g);
    print_first_sets(&g);
    
    return 0;
}