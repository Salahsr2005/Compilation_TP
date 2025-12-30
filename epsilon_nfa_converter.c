#include <stdio.h>

#define MAX_STATES 10
#define MAX_ALPHABET_SIZE 5

// Global configuration
int num_states, num_symbols;
char alphabet[MAX_ALPHABET_SIZE];

// Automaton properties
int initial_state;
int final_states[MAX_STATES];

// Transition storage
int symbol_transitions[MAX_STATES][MAX_ALPHABET_SIZE][MAX_STATES];
int epsilon_transitions[MAX_STATES][MAX_STATES];
int epsilon_closure[MAX_STATES][MAX_STATES];
int converted_transitions[MAX_STATES][MAX_ALPHABET_SIZE][MAX_STATES];

/**
 * Initialize all data structures to zero
 */
void initialize_data_structures(void) {
    for (int i = 0; i < MAX_STATES; i++) {
        final_states[i] = 0;
        
        for (int j = 0; j < MAX_ALPHABET_SIZE; j++) {
            for (int k = 0; k < MAX_STATES; k++) {
                symbol_transitions[i][j][k] = 0;
                converted_transitions[i][j][k] = 0;
            }
        }
        
        for (int j = 0; j < MAX_STATES; j++) {
            epsilon_transitions[i][j] = 0;
            epsilon_closure[i][j] = 0;
        }
    }
}

/**
 * Read automaton specification from user input
 */
void read_automaton_input(void) {
    int temp_state, final_count;
    char symbol;

    printf("Number of states: ");
    scanf("%d", &num_states);

    printf("Number of symbols: ");
    scanf("%d", &num_symbols);

    printf("Alphabet symbols: ");
    for (int i = 0; i < num_symbols; i++) {
        scanf(" %c", &alphabet[i]);
    }

    printf("Initial state: ");
    scanf("%d", &initial_state);

    printf("Number of final states: ");
    scanf("%d", &final_count);

    printf("Final states: ");
    for (int i = 0; i < final_count; i++) {
        scanf("%d", &temp_state);
        final_states[temp_state] = 1;
    }

    printf("Symbol transitions (-1 to stop):\n");
    while (1) {
        int from_state, to_state;
        scanf("%d", &from_state);
        
        if (from_state == -1) break;

        scanf(" %c %d", &symbol, &to_state);

        for (int i = 0; i < num_symbols; i++) {
            if (alphabet[i] == symbol) {
                symbol_transitions[from_state][i][to_state] = 1;
            }
        }
    }

    printf("Epsilon transitions (-1 to stop):\n");
    while (1) {
        int from_state, to_state;
        scanf("%d", &from_state);
        
        if (from_state == -1) break;

        scanf("%d", &to_state);
        epsilon_transitions[from_state][to_state] = 1;
    }
}

/**
 * Compute epsilon-closure for all states using transitive closure
 */
void compute_epsilon_closure(void) {
    int has_changes;

    // Initialize: each state can reach itself via epsilon
    for (int i = 0; i < num_states; i++) {
        epsilon_closure[i][i] = 1;
    }

    // Fixed-point iteration to compute transitive closure
    do {
        has_changes = 0;
        
        for (int i = 0; i < num_states; i++) {
            for (int j = 0; j < num_states; j++) {
                if (epsilon_closure[i][j]) {
                    for (int k = 0; k < num_states; k++) {
                        if (epsilon_transitions[j][k] && !epsilon_closure[i][k]) {
                            epsilon_closure[i][k] = 1;
                            has_changes = 1;
                        }
                    }
                }
            }
        }
    } while (has_changes);
}

/**
 * Remove epsilon transitions by computing new direct transitions
 */
void eliminate_epsilon_transitions(void) {
    for (int source = 0; source < num_states; source++) {
        for (int closure_state = 0; closure_state < num_states; closure_state++) {
            if (epsilon_closure[source][closure_state]) {
                for (int symbol_idx = 0; symbol_idx < num_symbols; symbol_idx++) {
                    for (int intermediate = 0; intermediate < num_states; intermediate++) {
                        if (symbol_transitions[closure_state][symbol_idx][intermediate]) {
                            for (int destination = 0; destination < num_states; destination++) {
                                if (epsilon_closure[intermediate][destination]) {
                                    converted_transitions[source][symbol_idx][destination] = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * Update final states based on epsilon-closure
 */
void update_final_states(void) {
    for (int i = 0; i < num_states; i++) {
        for (int j = 0; j < num_states; j++) {
            if (epsilon_closure[i][j] && final_states[j]) {
                final_states[i] = 1;
            }
        }
    }
}

/**
 * Display the resulting epsilon-free NFA
 */
void display_result(void) {
    printf("\n===== ε-free NFA =====\n");
    printf("Start state: %d\n", initial_state);

    printf("Final states: ");
    for (int i = 0; i < num_states; i++) {
        if (final_states[i]) {
            printf("%d ", i);
        }
    }
    printf("\n");

    printf("Transitions:\n");
    for (int i = 0; i < num_states; i++) {
        for (int j = 0; j < num_symbols; j++) {
            for (int k = 0; k < num_states; k++) {
                if (converted_transitions[i][j][k]) {
                    printf("%d --%c--> %d\n", i, alphabet[j], k);
                }
            }
        }
    }
}

/**
 * Main program execution
 */
int main(void) {
    initialize_data_structures();
    read_automaton_input();
    compute_epsilon_closure();
    eliminate_epsilon_transitions();
    update_final_states();
    display_result();
    
    return 0;
}