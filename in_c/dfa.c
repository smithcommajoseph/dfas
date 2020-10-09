// instructions regarding how to build/run/test coming soon...

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>	
#include <string.h>


// 
// Preprocessor directives
// 
#define DFA_SCANNER_INPUT_MAX 50
#define DFA_FILEPATH_MAX 100
#define DFA_STATE_MAX 50
#define DFA_INPUT_EDGE_MAX 2
#define YES 'y'
#define NO 'n'


// 
// String and string-ish Constants
// 
const char NEWLINE[] = "\n";
const char STR_SPLIT_TOKEN = ',';
const char CREATE_OR_LOAD_STR[] = "Please enter or load a DFA:%s1 - Enter DFA or 2 - Read DFA%s";
const char FILE_WRITE_ERR[] = "Sorry, no file was found at that path or new file was unable to be created.";
const char FILE_LOAD_ERR[] = "Sorry, no file was found at that path or it was unable to be processed.";
const char GENERIC_INPUT_ERR[] = "Sorry, that input didn't match what I was expecting, let's try again.";
const char INITIAL_STATE_ERR[] = "Provided initial state %s, is not in the set of known states%s";
const char ACCEPTING_STATE_ERR[] = "Provided accepting state %s, is not in the set of known states%s";
const char NOT_IN_ALPHABET_ERROR[] = "Sorry, %s is not in the DFA's alphabet.%s";
const char ENTER_STATES_STR[] = "Enter States (comma separated list):";
const char ENTER_INITIAL_STATE_STR[] = "Enter Initial state:";
const char ENTER_ACCEPTING_STATES_STR[] = "Enter accepting states (comma separated list):";
const char ENTER_TRANSITION_STR[] = "Enter a transition:";
const char DFA_FILE_PATH_STR[] = "Please enter a valid file path:";
const char DFA_TEST_STR[] = "Please enter an input string to test your DFA:";
const char ADD_TRANSITION_STR[] = "Would you like to enter another transition?";
const char WILL_SAVE_DFA_STR[] = "Would you like to save this DFA?";
const char WILL_TEST_AGAIN[] = "Would you like to test another string?";
const char ACCEPTED_STR[] = "Accepted!";
const char REJECTED_STR[] = "Rejected!";
const char Y_OR_N[] = "(y or n)";


// 
// Symbol typdef
// 
typedef char SYMBOL;


// 
// AlphabetItem linked list node struct
// 
struct AlphabetItem { 
    SYMBOL charVal[DFA_INPUT_EDGE_MAX];
    struct AlphabetItem* next; 
}; 


// 
// The Transition linked list node struct
// 
struct Transition { 
    char start[DFA_STATE_MAX],
         end[DFA_STATE_MAX]; 
    SYMBOL input[DFA_INPUT_EDGE_MAX];
    struct Transition* next; 
}; 


// 
// global DFA related vars
// 
char** states = NULL;
int statesCnt = 0;
char initialState[50];
char** acceptingStates = NULL;
int acceptingStatesCnt = 0;
struct AlphabetItem *headAlphabetItem = NULL;
struct AlphabetItem *currentAlphabetItem = NULL;
struct Transition *headTransition = NULL;
struct Transition *currentTransition = NULL;


/*
 * Clears the input buffers so as to to avoid carrying over input noise into 
 * subsequent scanner operations
 * (I had this in a previous c project, but I suspect I learned of this approach
 * somewhere on the internet - though I do not know where)
 *
 *  returns: void
 */
void clearInputBuffer () {
    while ((getchar()) != *NEWLINE); 
}


/*
 * Returns a count of a given char's appearance within a string
 *  haystack: the string we are evalating for instances of needle
 *  needle: the char we are looking for
 *
 *  returns: the number of times needle appears in haystack
 */
int countInstancesOf (char *haystack, char needle) { 
    int count = 0; 
    int i;

    for (i=0; haystack[i]; i++) {
        // checking character in string 
        if (haystack[i] == needle) {
            count++; 
        }
    }

    return count; 
}


/*
 * Returns a count of a given char's appearance within a string
 *  haystack: the string we are evalating for instances of needle
 *  haystackLen: the len of the haystack
 *  needle: the char we are looking for
 *
 *  returns: a boolean value indicating the existance of the needle in the 
 *      haystack
 */
bool isInArray (char **haystack, int haystackLen, char *needle) {
    bool retVal = false;
	int i;
	
    for(i = 0; i < haystackLen; i++) {
        if(strcmp(needle, haystack[i]) == 0) {
            retVal = true;
            break;
        }
    }   
    return retVal;
}


/*
 * Splits the given haystack string on the given needle, and populates the given
 * array based on the results
 * 
 * NOTE: Whole portions of this fn were obtained here, however, I perfomred 
 * both mechanical and functional modifications to use my countInstanceOf fn + 
 * 1 major windows-related bugfix (see line final *t = '\0'; stmt)
 * http://source-code-share.blogspot.com/2014/07/implementation-of-java-stringsplit.html
 *  inStr: string to be split
 *  delim: the item/token we are splitting the string on
 *  arr: the array that will store the split string items
 *
 *  returns: number of items in the resulting array
 */
int split (char *inStr, char delim, char ***arr) {
    int count = countInstancesOf(inStr, delim) + 1;
    int token_len = 1;
    int i = 0;
    char *p;
    char *t;

    *arr = (char**) malloc(sizeof(char*) * count);
    if (*arr == NULL) { exit(1); }

    p = inStr;
    while (*p != '\0') {
        if (*p == delim) {
            (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
            if ((*arr)[i] == NULL) { exit(1); }
            token_len = 0;
            i++;
        }
        p++;
        token_len++;
    }
    (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
    if ((*arr)[i] == NULL) { exit(1); }

    i = 0;
    p = inStr;
    t = ((*arr)[i]);
    while (*p != '\0') {
        if (*p != delim && *p != '\0') {
            *t = *p;
            t++;
        }
        else {
            *t = '\0';
            i++;
            t = ((*arr)[i]);
        }
        p++;
    }
    *t = '\0';

    return count;
}


/*
 * Checks to see if the given alphabet char exists in the AlphabetItem LL
 *  charVal: the char val (needle) we are searching for in the LL
 *
 *  returns: boolean depending on the existence of the incoming charVal
 */
bool doesAlphabetItemExist (char *charVal) {
    bool retVal = false;
    struct AlphabetItem *node = (struct AlphabetItem*) malloc(sizeof(struct AlphabetItem));

    if (headAlphabetItem != NULL) {
        node = headAlphabetItem;

        while (node->next != NULL) {
            if (strcmp(node->charVal, charVal) == 0) { 
                retVal = true;
                break;
            }
            node = node->next;
        };
        // don't forget to check the final item in the LL
        if (strcmp(node->charVal, charVal) == 0) { 
            retVal = true;
        }        
    }

    return retVal;
}


/*
 * Inserts a new AlphabetItem in the LL
 *  charVal: the char val to be added to the DFA's alphabet
 *
 *  returns: void
 */
void addAlphabetItem (char *charVal) {
    struct AlphabetItem *node = (struct AlphabetItem*) malloc(sizeof(struct AlphabetItem));

    strcpy(node->charVal, charVal);
    node->next = NULL;

    if (headAlphabetItem == NULL) {
        headAlphabetItem = currentAlphabetItem = node;
    }
    else {
        currentAlphabetItem->next = node;      
        currentAlphabetItem = node;
    }
}


/*
 * Inserts a new Transition item in the LL
 *  start: starting state of the incoming transition
 *  input: incoming input to be evaluated
 *  end: ending state of the incoming transition
 *
 *  returns: void
 */
void addTransition (char *start, char *input, char *end) {
    struct Transition *node = (struct Transition*) malloc(sizeof(struct Transition));

    strcpy(node->start, start);
    strcpy(node->input, input);
    strcpy(node->end, end);
    node->next = NULL;

    if (headTransition == NULL) {
        headTransition = currentTransition = node;
    }
    else {
        currentTransition->next = node;      
        currentTransition = node;
    }
}


/*
 * Returns the current 'active' transition struct based on start and input values
 *  start: target start value for the transition we wish to find
 *  testChar: target input value for the transition we wish to find, this is
 *  provided to this fn as a char massaged into a string for comparison
 *
 *  returns: a transition matching the current start and test values provided
 */
struct Transition* getActiveTransition (char* start, char testChar) {  
    struct Transition* current = headTransition;
    struct Transition* retVal = NULL;
    char testStr[1];
    testStr[0] = testChar;

    if (doesAlphabetItemExist(testStr) == true) {
        while (current != NULL) {  
            if (strcmp(current->start, start) == 0 
            && strcmp(current->input, testStr) == 0 ) {
                retVal = current;
                break;
            }

            current = current->next;
        }       
    } 
    else {
        printf(NOT_IN_ALPHABET_ERROR, testStr, NEWLINE);
    }

    return retVal;  
}


/*
 * Update states with incoming states value
 * NOTE: needs validation / err workflows
 *  statesStr: comma separated list of DFA states
 *
 *  returns: boolean success val
 */
bool updateStates (char* statesStr) {
    bool retVal = true;
    split(statesStr, STR_SPLIT_TOKEN, &states);   
    statesCnt = countInstancesOf(statesStr, STR_SPLIT_TOKEN) + 1;
    return retVal;
}


/*
 * Update initial state with incoming initial states value
 *  initialStateStr: incoming initial state
 *
 *  returns: boolean success val
 */
bool updateInitialState (char* initialStateStr) {
    bool retVal = true;
    strcpy(initialState, initialStateStr);
    if (isInArray(states, statesCnt, initialState) != true) {
        printf(INITIAL_STATE_ERR, initialState, NEWLINE);
        retVal = false;
    }
    return retVal;
}


/*
 * Update accepting state with incoming ccepting states value
 *  acceptingStatesStr: accepting state
 *
 *  returns: boolean success val
 */
bool updateAcceptingStates (char* acceptingStatesStr) {
    bool retVal = true;
	int i;
	
    split(acceptingStatesStr, STR_SPLIT_TOKEN, &acceptingStates);
    acceptingStatesCnt = countInstancesOf(acceptingStatesStr, STR_SPLIT_TOKEN) + 1;

    for (i = 0; i < acceptingStatesCnt; i++) {
        if (isInArray(states, statesCnt, acceptingStates[i]) != true) {
            printf(ACCEPTING_STATE_ERR, acceptingStates[i], NEWLINE);
            retVal = false;
        }
    }

    return retVal;
}


/*
 * Parses and adds incoming transition - updates the list of valid alphabet items
 * as well if appropriate
 * NOTE: needs validation / err workflows. should verify number of items (3) etc
 *  transitionsStr: comma separated list of transition props (start,input,end)
 *
 *  returns: boolean success val
 */
bool updateTransitions (char* transitionsStr) {
    bool retVal = true;
    char** tmpTransitionVals = NULL;    

    split(transitionsStr, STR_SPLIT_TOKEN, &tmpTransitionVals);    
    addTransition(tmpTransitionVals[0], tmpTransitionVals[1], tmpTransitionVals[2]);

    if (doesAlphabetItemExist(tmpTransitionVals[1]) == false) {
        addAlphabetItem(tmpTransitionVals[1]);
    }

    return retVal;
}


/*
 * Prompts the user for input and updates states accordingly
 *
 *  returns: void
 *
 *  returns: boolean success val
 */
void promptStates () {
    bool success;
    char statesStr[DFA_SCANNER_INPUT_MAX];

    printf("%s%s", ENTER_STATES_STR, NEWLINE);
    scanf("%s", statesStr);
    clearInputBuffer();

    success = updateStates(statesStr);
    if (success == false) {
        printf("%s%s", GENERIC_INPUT_ERR, NEWLINE);
        promptStates();
    }
}


/*
 * Prompts the user for input and updates initial state accordingly
 *
 *  returns: void
 */
void promptInitialState () {
    bool success;
    char tmpinitialState[50];

    printf("%s%s", ENTER_INITIAL_STATE_STR, NEWLINE);
    scanf("%s", tmpinitialState);
    clearInputBuffer();
    success = updateInitialState(tmpinitialState);

    if (success == false) {
        printf("%s%s", GENERIC_INPUT_ERR, NEWLINE);
        promptInitialState();
    }    
}


/*
 * Prompts the user for input and updates accepting state accordingly
 *
 *  returns: void
 */
void promptAcceptingStates () {
    bool success;
    char acceptingStatesStr[DFA_SCANNER_INPUT_MAX];

    printf("%s%s", ENTER_ACCEPTING_STATES_STR, NEWLINE);
    scanf("%s", acceptingStatesStr);
    clearInputBuffer();

    success = updateAcceptingStates(acceptingStatesStr);

    if (success == false) {
        printf("%s%s", GENERIC_INPUT_ERR, NEWLINE);
        promptAcceptingStates();
    }      
}


/*
 * Prompts the user for input and updates transitions accordingly
 *
 *  returns: void
 */
void promptTransitions () {
    char transitionsStr[DFA_SCANNER_INPUT_MAX],
         isAddingTransitions;

    while (isAddingTransitions != 'n') {
        printf("%s%s", ENTER_TRANSITION_STR, NEWLINE);
        scanf("%s", transitionsStr);
        clearInputBuffer();

        updateTransitions(transitionsStr);

        printf("%s %s %s", ADD_TRANSITION_STR, Y_OR_N, NEWLINE);
        scanf(" %c", &isAddingTransitions);
        clearInputBuffer();
    }     
}


/*
 * Writes the DFA in memory to a file
 *
 *  returns: void
 */
void writeDFA () {
    char filePath[DFA_FILEPATH_MAX];
    
    printf("%s%s", DFA_FILE_PATH_STR, NEWLINE);
    scanf("%s", filePath);
    clearInputBuffer();

    FILE *dfaFile = fopen(filePath, "w+");
	
	int i;
	
    if (dfaFile != NULL) {
        // states
        for (i = 0; i < statesCnt; i++) {
            if (i != statesCnt - 1) { fprintf(dfaFile, "%s,", states[i]); }
            else { fprintf(dfaFile, "%s%s", states[i], NEWLINE); }
        }

        // initial state
        fprintf(dfaFile, "%s%s", initialState, NEWLINE);
		
		int i;
		
        // accepting states
        for (i = 0; i < acceptingStatesCnt; i++) {
            if (i != acceptingStatesCnt - 1) { fprintf(dfaFile, "%s,", acceptingStates[i]); }
            else { fprintf(dfaFile, "%s%s", acceptingStates[i], NEWLINE); }
        }

        // transitions
        struct Transition *ptr = headTransition;

        //start from the beginning
        while(ptr != NULL) {
            fprintf(dfaFile, "%s,%s,%s", ptr->start, ptr->input, ptr->end);
            if (ptr->next != NULL) { fprintf(dfaFile, NEWLINE); }
            ptr = ptr->next;
        }
    }

    fclose(dfaFile);
}


/*
 * Prompts the user to save (or not) the current DFA
 *
 *  returns: void
 */
void promptWriteDFA () {
    char willSave = NULL;

    while (willSave != YES && willSave != NO) {
        printf("%s %s %s", WILL_SAVE_DFA_STR, Y_OR_N, NEWLINE);
        scanf(" %c", &willSave);
        clearInputBuffer();
        if (willSave == YES) { writeDFA(); }
    }  
}


/*
 * Provides the user a series of prompts and creates a DFA 
 * NOTE: no useful validation has been implemented
 *
 *  returns: void
 */
void createDFA () {
    promptStates();
    promptInitialState();
    promptAcceptingStates();
    promptTransitions();
    promptWriteDFA();
}


/*
 * Prompts the user for a file path, loads the file, and loads the values accordingly
 * so we may have a functional DFA to test values against
 * NOTE: Much of the file loading related code has been informed or copied from 
 * this SO answer https://stackoverflow.com/a/2029227/257476. I found simpler 
 * examples of this type of workflow, but felt like this pattern did a fine job
 * accounting for various error states 
 *
 *  returns: void
 */
void loadDFA () {
    char filePath[DFA_FILEPATH_MAX];
    bool success;

    printf("%s%s", DFA_FILE_PATH_STR, NEWLINE);
    scanf("%s", filePath);  
    clearInputBuffer();
    
    char* src = NULL;
    FILE* dfaFile = fopen(filePath, "r");

    if (dfaFile != NULL) {
        if (fseek(dfaFile, 0L, SEEK_END) == 0) {
            long bufsize = ftell(dfaFile);
            if (bufsize == -1) { 
                printf("%s%s", FILE_LOAD_ERR, NEWLINE);
                loadDFA();
            }

            src = (char*) malloc(sizeof(char) * (bufsize + 1));

            // Start at the beginning
            if (fseek(dfaFile, 0L, SEEK_SET) != 0) { 
                printf("%s%s", FILE_LOAD_ERR, NEWLINE);
                loadDFA();
            }

            // Read the file into memory.
            size_t newLen = fread(src, sizeof(char), bufsize, dfaFile);
            if (ferror(dfaFile) != 0) {
                printf("%s%s", FILE_LOAD_ERR, NEWLINE);
                loadDFA();
            } else {
                src[newLen++] = '\0';
            }
        }
        fclose(dfaFile);
    }
    else {
        printf("%s%s", FILE_LOAD_ERR, NEWLINE);
        loadDFA();
    }
    
    char* row = strtok(src, NEWLINE);
    int lineCnt = 1;
    
    while (row != NULL) {
        if      (lineCnt == 1) { success = updateStates(row); }
        else if (lineCnt == 2) { success = updateInitialState(row); }
        else if (lineCnt == 3) { success = updateAcceptingStates(row); }      
        else                   { success = updateTransitions(row); }  
        
        if (success == false) { break; }
        row = strtok(NULL, NEWLINE);
        lineCnt++;
    }
    
    if (success == false) {
        printf("%s%s", FILE_LOAD_ERR, NEWLINE);
        loadDFA();        
    }

    free(src);
}


/*
 * Fork in the path, we can either load a DFA or create one
 *
 *  returns: void
 */
void getDFA () {
    int createOrLoad;

    printf(CREATE_OR_LOAD_STR, NEWLINE, NEWLINE);
    scanf("%d", &createOrLoad);
    clearInputBuffer();

    if      (createOrLoad == 1) { createDFA(); }
    else if (createOrLoad == 2) { loadDFA(); }
    else    { getDFA(); }
}


/*
 * Prompts the user for input, then tests the provided string(s) against the 
 * DFA in memory
 *
 *  returns: void
 */
void runDFA () {
    struct Transition* currentTransition = NULL;
    char* currentState = initialState;
    char testString[DFA_SCANNER_INPUT_MAX];
    char testAgain = NULL;

    printf("%s%s", DFA_TEST_STR, NEWLINE);
    scanf("%s", testString);
    clearInputBuffer();

    int len = strlen(testString);
    int i;
    for (i = 0; i < strlen(testString); i++){
        currentTransition = getActiveTransition(currentState, testString[i]);
        if (currentTransition != NULL) {
            currentState = currentTransition->end;
        }
    }

    if (currentTransition != NULL
    && isInArray(acceptingStates, acceptingStatesCnt, currentState) == true) {
        printf("%s%s", ACCEPTED_STR, NEWLINE);
    } 
    else {
        printf("%s%s", REJECTED_STR, NEWLINE);
    }

    while (testAgain != YES || testAgain != NO) {
        printf("%s %s %s", WILL_TEST_AGAIN, Y_OR_N, NEWLINE);
        scanf(" %c", &testAgain);
        clearInputBuffer();

        if      (testAgain == YES) { runDFA(); }
        else if (testAgain == NO)  { exit(0); }
    }   
}


/*
 * Calls all relevant functions and does the things
 * 
 *  returns: the exit code - 0 if all is well, !0 if errors occur
 */
int main (void) {
    getDFA();
    runDFA();
}