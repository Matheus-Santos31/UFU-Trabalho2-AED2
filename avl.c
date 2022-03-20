#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

typedef struct avl_tree avl_tree;
typedef struct avl_node avl_node;

struct avl_node{
	char *word;
	int *lines;

	int size_word;
	int size_lines;

	int value;

	struct avl_node *left;
	struct avl_node *right;
};

struct avl_tree{
	struct avl_node *root;
};

avl_tree* avl_create(){
	avl_tree *tree = malloc(sizeof(avl_tree));
	tree->root = NULL;

	return tree;
}

avl_node* avl_create_node(){
	avl_node *node = malloc(sizeof(avl_node));
	node->left  = NULL;
	node->right = NULL;
	node->word   = NULL;
	node->lines  = NULL;
	node->size_word  = 0;
	node->size_lines = 0;

	return node;
}

int avl_node_height(avl_node *node){
	int height_left  = 0;
	int height_right = 0;

	if(node->left){
		height_left = avl_node_height(node->left);
	}

	if(node->right){
		height_right = avl_node_height(node->right);
	}

	if(height_right > height_left){
		return ++height_right;
	}else {
		return ++height_left;
	}
}

int avl_balance_factor(avl_node *node){
	int balance_factor = 0;

	if(node->left){
		balance_factor += avl_node_height(node->left);
	}

	if(node->right){
		balance_factor -= avl_node_height(node->right);
	}

	return balance_factor;
}

avl_node* avl_rotate_leftleft(avl_node *node){
	avl_node *a = node;
	avl_node *b = a->left;

	a->left  = b->right;
	b->right = a;

	return b;
}

avl_node* avl_rotate_leftright(avl_node *node){
	avl_node *a = node;
	avl_node *b = a->left;
	avl_node *c = b->right;

	a->left = c->right;
	b->right  = c->left;
	c->left = b;
	c->right  = a;

	return c;
}

avl_node* avl_rotate_rightleft(avl_node *node){
	avl_node *a = node;
	avl_node *b = a->right;
	avl_node *c = b->left;

	a->right = c->left;
	b->left  = c->right;
	c->right = b;
	c->left  = a;

	return c;
}

avl_node* avl_rotate_rightright(avl_node *node){
	avl_node *a = node;
	avl_node *b = a->right;

	a->right  = b->left;
	b->left = a;

	return b;
}

avl_node* avl_balance_node(avl_node *node){
	avl_node *new_root = NULL;

	if(node->left){
		node->left = avl_balance_node(node->left);
	}

	if(node->right){
		node->right = avl_balance_node(node->right);
	}

	int balance_factor = avl_balance_factor(node);

	if(balance_factor >= 2){

		if(avl_balance_factor(node->left) <= -1){
			new_root = avl_rotate_leftright(node);
		}else {
			new_root = avl_rotate_leftleft(node);
		}

	}else if(balance_factor <= -2){
		
		if(avl_balance_factor(node->right) >= 1){
			new_root = avl_rotate_rightleft(node);
		}else {
			new_root = avl_rotate_rightright(node);
		}

	}else {
		new_root = node;
	}

	return new_root;
}

void avl_balance(avl_tree *tree){
	avl_node *new_root = NULL;

	new_root = avl_balance_node(tree->root);

	if(new_root != tree->root){
		tree->root = new_root;
	}
}

avl_node* find_node(avl_tree* tree, char *word, int size_word){

	avl_node *current = tree->root;

	while(current && strncmp(word, current->word, size_word) != 0){
		if(strncmp(word, current->word, size_word) > 0){
			current = current->right;
		}else {
			current = current->left;
		}
	}

	return current;
	
}

void update_node(avl_node *node, int line){
	node->lines = realloc(node->lines, ++node->size_lines * sizeof(int));
	node->lines[node->size_lines - 1] = line;
}

void avl_insert(avl_tree *tree, char *word, int size_word, int line){
	avl_node *node_update = find_node(tree, word, size_word);

	if(node_update == NULL){
		avl_node *node = NULL;
		avl_node *next = NULL;
		avl_node *last = NULL;

		if(tree->root == NULL){
			node = avl_create_node();

			node->word  = malloc(size_word);
			node->lines = malloc(sizeof(int));

			strncpy(node->word, word, size_word);
			node->lines[0] = line;

			node->size_word  = size_word;
			node->size_lines = 1;

			tree->root = node;
			
		}else {
			next = tree->root;

			while(next != NULL){
				last = next;

				if(strcmp(word, next->word) < 0){
					next = next->left;
				}else if(strcmp(word, next->word) > 0){
					next = next->right;
				}
			}

			node = avl_create_node();

			node->word  = malloc(size_word);
			node->lines = malloc(sizeof(int));

			strncpy(node->word, word, size_word);
			node->lines[0] = line;

			node->size_word  = size_word;
			node->size_lines = 1;

			if(strcmp(word, last->word) < 0){
				last->left = node;
			}

			if(strcmp(word, last->word) > 0){
				last->right = node;
			}
		}

		avl_balance(tree);

	}else {
		update_node(node_update, line);

	}
}

void in_order_node(avl_node *node, FILE* resultado){
	if(node == NULL){
		return;
	}

	in_order_node(node->left, resultado);

	fprintf(resultado, "%s [", node->word);
	for (int i = 0; i < node->size_lines - 1; ++i){
		fprintf(resultado, "%i, ", node->lines[i]);
	}
	fprintf(resultado, "%i]\n", node->lines[node->size_lines - 1]);
	
	in_order_node(node->right, resultado);
}

void in_order_tree(avl_tree *tree, FILE* resultado){
	in_order_node(tree->root, resultado);
}

int is_delimiter(char c){
	char delimiter[] = "\0\n .!?:-/\\()*";
	int size_delimiter = sizeof(delimiter);

	for (int i = 0; i < size_delimiter; ++i){
		if(c == delimiter[i]){
			return 1;
		}
	}

	return 0;
}

void count(avl_node *node, int *total, int *distintas){
	if(node == NULL){
		return;
	}

	count(node->left, total, distintas);
	
	(*distintas)++;
	(*total) += node->size_lines; 

	count(node->right, total, distintas);
}

int main(){
	FILE *text  = fopen("livro.txt", "r+");
	FILE *resultado = fopen("resultado.txt", "w+");

	int line = 1;
	char letter;
	char *word = NULL;
	int size_word = 0;

	clock_t t = clock();
	avl_tree *tree = avl_create();

	while((letter = fgetc(text)) != EOF){
		if(is_delimiter(letter)){
			if(word != NULL){
				avl_insert(tree, word, size_word, line);
				size_word = 0;
			}

			if(letter == '\n'){
				line++;
			}
			
		}else {
			word = realloc(word, ++size_word);

			if(isalpha(letter)){
				word[size_word - 1] = tolower(letter);
			}else {
				word[size_word - 1] = letter;
			}
		}
	}

	if(word != NULL){
		avl_insert(tree, word, size_word, line);
		size_word = 0;
	}

	if(letter == '\n'){
		line++;
	}

	t = clock() - t;

	in_order_tree(tree, resultado);

	int total = 0;
	int distintas = 0;

	count(tree->root, &total, &distintas);
	fprintf(resultado, "total: %i,  distintas: %i\n", total, distintas);
	fprintf (resultado, "Tempo: %f seconds\n",((float)t)/CLOCKS_PER_SEC);

	return 0;
}