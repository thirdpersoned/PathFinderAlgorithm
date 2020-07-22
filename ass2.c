/* Solution to comp10002 Assignment 2, 2019 semester 2.

   Authorship Declaration:

   (1) I certify that the program contained in this submission is completely
   my own individual work, except where explicitly noted by comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students,
   or by non-students as a result of request, solicitation, or payment,
   may not be submitted for assessment in this subject.  I understand that
   submitting for assessment work developed by or in collaboration with
   other students or non-students constitutes Academic Misconduct, and
   may be penalized by mark deductions, or by other penalties determined
   via the University of Melbourne Academic Honesty Policy, as described
   at https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will
   I do so until after the marks are released. I understand that providing
   my work to other students, regardless of my intention or any undertakings
   made to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring
   service, or drawing the attention of others to such services and code
   that may have been made available via such a service, may be regarded
   as Student General Misconduct (interfering with the teaching activities
   of the University and/or inciting others to commit Academic Misconduct).
   I understand that an allegation of Student General Misconduct may arise
   regardless of whether or not I personally make use of such solutions
   or sought benefit from such actions.

   Signed by: Gabriel George Jackson 983339
   Dated:     21/10/2019

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define NOBLOCK -1 // Field types
#define BLOCK -2
#define INITIAL -3
#define GOAL -4

#define NOSTATUS 0 // Checks status

#define INPUTTRUE 2 // Make sure that scan is working

#define ONEBLOCK 1 // Move to adjacent square
#define NOTBLOCK 0 // Not adjacent square

#define INITIALLN 1 // Work around margins

#define COORDPERLINE 5 // Printing coords per line
#define NORMDR 0

#define MARGIN 1 // Add margins

#define ABLK 1 // Check adjacent block
#define ISRT 0 // Check that field point is on the route

#define TRUE 1
#define FALSE 0

#define VALINC 1 // Increment value

#define BRDR 0 // Check border

#define ONEDIGIT 10 // Print one's digit

// Define node and coordinate structs
// mostly taken from Alistair Moffat
typedef struct path_node node_t;
typedef struct coord data_t;

struct coord {
    int r, c;
};

struct path_node {
    data_t coord;
    node_t *next;
};

// Linked list path
typedef struct {
    node_t *head;
    node_t *foot;
} path_t;

path_t *make_empty_list(void);
path_t *add_path_node(path_t *path, int r, int c);
int **add_blocks(int **field, int *blkcnt);
path_t *add_path(path_t *path);
void print_path(path_t *path);
int print_status(path_t *path, int **field, int ir, int ic, int gr, int gc);
void print_grid(path_t *path, int **field, int r, int c);
int **expand(int **field, int r, int c, int *val);
int check_connect(int **field, node_t *node, node_t **enode);
node_t *insert_path_node(node_t **snode, node_t *enode, int r, int c);
void traverse(int **field, node_t **snode, node_t *enode, int r, int c);
void free_path(path_t *path);
void free_section(path_t *path, node_t *start, node_t *end);

// ONLY WORKS FOR STAGE 0 AND 1
int
main(int argc, char *argv[]) {
    // Initialise row, column, block count, 'I' row, 'I' column, 'G' row,
    // 'G' column, status type and path.
    int r = 0, c = 0, blkcnt = 0, ir = -1, ic = -1, gr = -1, gc = -1,
    status = 0, i = 0, j = 0;
    path_t *path = NULL;

    printf("==STAGE 0=======================================\n");
    scanf("%dx%d\n", &r, &c); // Get grid size
    printf("The grid has %d rows and %d columns.\n", r, c);

    /* Create 2-D array */
    int **field = (int **)malloc(r * sizeof(int *));
    assert(field != NULL);
    for (i = 0; i < r; i++) {
        field[i] = (int *)malloc(c * sizeof(int));
        assert(field[i] != NULL);
        for (j = 0; j < c; j++) {
            // Assign each field to no block initially
            field[i][j] = NOBLOCK;
        }
    }

    scanf("[%d,%d]\n", &ir, &ic); // Get initial route point
    field[ir][ic] = INITIAL;
    scanf("[%d,%d]\n", &gr, &gc); // Get goal route point
    field[gr][gc] = GOAL;

    field = add_blocks(field, &blkcnt); // Get block count and add blocks

    printf("The grid has %d block(s).\n", blkcnt);
    printf("The initial cell in the grid is [%d,%d].\n", ir, ic);
    printf("The goal cell in the grid is [%d,%d].\n", gr, gc);
    printf("The proposed route in the grid is:\n");
    scanf("$\n[");

    path = make_empty_list(); // Begin linked list and initialise path
    path = add_path(path);
    print_path(path);
    status = print_status(path, field, ir, ic, gr, gc); // Get and print status
    printf("==STAGE 1=======================================\n");

    print_grid(path, field, r, c); // Print status
    if (status) { // If it's not a valid route
        printf("------------------------------------------------\n");
        int val = 0, lncnnct = FALSE;
        node_t *node = path->head, *enode = NULL; // initialise route head and
        // end node (enode)
        while (node->next) { // Go through linked list and check where block is
            int nxtr = node->next->coord.r, nxtc = node->next->coord.c;
            if (field[nxtr][nxtc] == BLOCK) {
                field[node->coord.r][node->coord.c] = val; // Give initial val 0
                break;
            }
            node = node->next;
        }
        while (!lncnnct && (val < r*c)) { // check if there is a route connect
            // and if not exit
            field = expand(field, r, c, &val); // flood
            lncnnct = check_connect(field, node, &enode); // check route
        }

        if (val < r*c) { // if there is a route change linked list
            // free the blocked section
            free_section(path, node, enode);
            traverse(field, &node, enode, r, c);
        }

        print_grid(path, field, r, c);
        printf("------------------------------------------------\n");
        print_path(path);
        if (val < r*c) { // if route found
            status = print_status(path, field, ir, ic, gr, gc);
        } else {
            printf("The route cannot be repaired!\n");
        }
    }

    printf("================================================\n");

    // free path
    free_path(path);

    // free field
    for (i = 0; i < r; i++) {
        free(field[i]);
    }
    free(field);

    return 0;
}

path_t *make_empty_list(void) {
    // make an empty linked list, mostly taken from Alistair Moffat
    path_t *path = NULL;
    path = (path_t*)malloc(sizeof(*path));
    assert(path != NULL);
    path->head = path->foot = NULL;
    return path;
}

path_t *add_path_node(path_t *path, int r, int c) {
    // add a node to foot of the linked list, with assigned coords
    // mostly taken from Alistair Moffat
    node_t *new = NULL;
    new = (node_t*)malloc(sizeof(*new));
    assert(new != NULL);
    new->coord.r = r;
    new->coord.c = c;
    new->next = NULL;
    // if initial node in list
    if (path->foot==NULL) {
        path->head = path->foot = new;
    } else {
        path->foot->next = new;
        path->foot = new;
    }
    return path;
}

void free_path(path_t *path) {
    // free linked list
    node_t *tmp = NULL;
    while (path->head) {
        tmp = path->head;
        path->head = tmp->next;
        free(tmp);
    }
    free(path);
}

void free_section(path_t *path, node_t *start, node_t *end) {
    // free the blocked section that will be over written
    node_t *tmp = NULL;
    while (start->next != end) {
        tmp = start->next;
        start->next = tmp->next;
        free(tmp);
    }
}

int **add_blocks(int **field, int *blkcnt) {
    int posr = 0, posc = 0;
    // scan in blocks and increment block count
    while (scanf("[%d,%d]\n", &posr, &posc) == INPUTTRUE) {
        *blkcnt = *blkcnt + VALINC;
        field[posr][posc] = BLOCK;
    }
    return field;
}

path_t *add_path(path_t *path) {
    int posr = 0, posc = 0, c = 0;
    // go through route and add to linked list
    while (scanf("%d,%d]->", &posr, &posc) == INPUTTRUE) {
        path = add_path_node(path, posr, posc);
        // skip over chars until at '['
        while (!((c = getchar()) == EOF || (char)c == '[')) {
        }
    }
    return path;
}

void print_path(path_t *path) {
    node_t *node = path->head;
    int lncnt = INITIALLN;
    // go through linked list and print nodes
    while (node->next) {
        printf("[%d,%d]->", node->coord.r, node->coord.c);
        node = node->next;
        // print only 5 coords per line
        if (lncnt % COORDPERLINE == NORMDR) {
            printf("\n");
        }
        lncnt++;
    }
    // final node
    printf("[%d,%d].\n", node->coord.r, node->coord.c);
}

int print_status(path_t *path, int **field, int ir, int ic, int gr, int gc) {
    node_t *node = path->head;
    int status = NOSTATUS, lstr = 0, lstc = 0;
    // check initial cell at head node
    if (!(node->coord.r == ir && node->coord.c == ic) ||
        field[ir][ic] == BLOCK) {
        printf("Initial cell in the route is wrong!\n");
        status++;
    }
    node = path->foot;
    // check goal cell at end node
    if ((!(node->coord.r == gr && node->coord.c == gc) ||
        field[gr][gc] == BLOCK) && !status) {
        printf("Goal cell in the route is wrong!\n");
        status++;
    }
    node = path->head;
    lstr = node->coord.r, lstc = node->coord.c;
    // go through linked list to check for illegal moves and blocked routes
    while (node->next && !(status)) {
        node = node->next;
        int rdif = lstr - node->coord.r, cdif = lstc - node->coord.c;
        // check for only 1 adjacent moves
        if (!(((rdif)*(rdif) == ONEBLOCK && (cdif) == NOTBLOCK)
              || ((rdif) == NOTBLOCK && (cdif)*(cdif) == ONEBLOCK)
              || ((rdif) == NOTBLOCK && (cdif) == NOTBLOCK))) {
            printf("There is an illegal move in this route!\n");
            status++;
        // check for blocked nodes
        } else if (field[node->coord.r][node->coord.c] == BLOCK) {
            printf("There is a block on this route!\n");
            status++;
        }
        // store last node to check for adjacent moves
        lstr = node->coord.r, lstc = node->coord.c;
    }
    // if there is no status print valid route
    if (!status) {
        printf("The route is valid!\n");
    }
    return status;
}

void print_grid(path_t *path, int **field, int r, int c) {
    node_t *node = path->head;
    int nodebool = FALSE, i = 0, j = 0;
    // go through field
    for (i = 0; i < r + MARGIN; i++) {
        for (j = 0; j < c + MARGIN; j++) {
            // print margins
            if (i == BRDR && j > BRDR) {
                printf("%d", (j - MARGIN) % ONEDIGIT); // print only one's digit
            } else if (j == BRDR) {
                if (i > BRDR) {
                    printf("\n%d", (i - MARGIN) % ONEDIGIT); // "" ""
                } else {
                    printf(" ");
                }
            } else if (field[i - MARGIN][j - MARGIN] == INITIAL) {
                printf("I"); // Print Initial cell
            } else if (field[i - MARGIN][j - MARGIN] == GOAL) {
                printf("G"); // Print Goal cell
            } else if (field[i - MARGIN][j - MARGIN] == BLOCK) {
                printf("#"); // Print Blocks
            } else {
                nodebool = FALSE;
                node = path->head;
                // Go through linked list to check for route
                while (node->next) {
                    if (node->coord.r == i - MARGIN && node->coord.c == j - MARGIN) {
                        printf("*");
                        nodebool = TRUE;
                        break;
                    }
                    node = node->next;
                }
                // If no node found just print " "
                if (!nodebool) {
                    printf(" ");
                }
            }
        }
    }
    printf("\n");
}

int** expand(int **field, int r, int c, int *val) {
    int i = 0, j = 0;
    // Go through the field, expand flood to necessary cells 'till route found
    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            if (field[i][j] == *val) {
                // Expand up
                if (i - ABLK >= ISRT && !(field[i - ABLK][j] <= BLOCK ||
                    (field[i - ABLK][j] < *val && field[i - ABLK][j] >= ISRT))){
                    field[i - ABLK][j] = *val + VALINC;
                }
                // Expand down
                if (i + ABLK < r && !(field[i + ABLK][j] <= BLOCK ||
                    (field[i + ABLK][j] < *val && field[i + ABLK][j] >= ISRT))){
                    field[i + ABLK][j] = *val + VALINC;
                }
                // Expand left
                if (j - ABLK >= ISRT && !(field[i][j - ABLK] <= BLOCK ||
                    (field[i][j - ABLK] < *val && field[i][j - ABLK] >= ISRT))){
                    field[i][j - ABLK] = *val + VALINC;
                }
                // Expand right
                if (j + ABLK < c && !(field[i][j + ABLK] <= BLOCK ||
                    (field[i][j + ABLK] < *val && field[i][j + ABLK] >= ISRT))){
                    field[i][j + ABLK] = *val + VALINC;
                }
            }
        }
    }
    // Increment flood value
    *val = *val + VALINC;
    return field;
}

int check_connect(int **field, node_t *node, node_t **enode) {
    node_t *chk_node = node;
    int found = FALSE;
    // Check each node to make sure it hits a flood value
    while (chk_node->next) {
        if (field[chk_node->coord.r][chk_node->coord.c] == BLOCK) {
            // Skip blocks
        } else if (field[chk_node->coord.r][chk_node->coord.c] > ISRT) {
            // If it founds a value in the field, save end node, closest to end
            *enode = chk_node;
            found = TRUE;
        }
        chk_node = chk_node->next;
    }
    // Return found end node or not
    if (found) {
        return TRUE;
    }
    return FALSE;
}

node_t *insert_path_node(node_t **snode, node_t *enode, int r, int c) {
    // Injects a node into the linked list to fix path
    node_t *new = NULL;
    new = (node_t*)malloc(sizeof(*new));
    assert(new != NULL);
    new->coord.r = r;
    new->coord.c = c;
    // Inject node to next to be the end node, start node to point to new node
    new->next = enode;
    (*snode)->next = new;
    return new;
}

void traverse(int **field, node_t **snode, node_t *enode, int r, int c) {
    int travr = enode->coord.r, travc = enode->coord.c,
    rdif = (*snode)->coord.r - enode->coord.r,
    cdif = (*snode)->coord.c - enode->coord.c;
    // New node to be added
    node_t *travnode;
    // Check the end node and start node to make sure new node is
    // more than 1 block away.
    if (!(((rdif)*(rdif) == ONEBLOCK && (cdif) == NOTBLOCK)
          || ((rdif) == NOTBLOCK && (cdif)*(cdif) == ONEBLOCK))) {
        // Check up
        if (travr - ABLK >= ISRT && field[travr - ABLK][travc] < field[travr][travc]
            && field[travr - ABLK][travc] >= ISRT) {
            travnode = insert_path_node(snode, enode, travr - 1, travc);
        // Check down
        } else if (travr + ABLK < r &&
                   field[travr + ABLK][travc] < field[travr][travc]
                   && field[travr + ABLK][travc] >= ISRT) {
            travnode = insert_path_node(snode, enode, travr + ABLK, travc);
        // Check left
        } else if (travc - ABLK >= ISRT &&
                   (field[travr][travc - ABLK] < field[travr][travc]
                    && field[travr][travc - ABLK] >= ISRT)) {
            travnode = insert_path_node(snode, enode, travr, travc - ABLK);
        // Check right
        } else if (travc + ABLK < c &&
                   field[travr][travc + ABLK] < field[travr][travc]
                   && field[travr][travc + ABLK] >= ISRT) {
            travnode = insert_path_node(snode, enode, travr, travc + ABLK);
        }
        // Use recursion to traverse the grid, making the new node the end node
        // as to inject the new node after the start node and the end node, or
        // newly inserted node
        traverse(field, snode, travnode, r, c);
    }
}
// Algorithms areYeah fun!!