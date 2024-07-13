#include <assert.h>
typedef struct _TreeNode {
  int lo, hi;
  void *data;
  struct _TreeNode *left;
  struct _TreeNode
      *right;  //	int (*read)(void *data, void *ptr, unsigned int size);

  //	TreeNode(*newTree)int x) : val(x), left(NULL), right(NULL) {}
} TreeNode;
TreeNode *newTree(int lo, int hi, void *data) {
  TreeNode *t = (TreeNode *)malloc(sizeof(t));
  t->lo = lo;
  t->hi = hi;
  t->left = NULL;
  t->right = NULL;
  t->data = data;
  return t;
}
void insert(TreeNode **root, TreeNode *t) {
  if (*root == NULL) {
    *root = t;
    return;
  }
  if ((*root)->left == NULL || t->hi < (*root)->left->hi) {
    insert(&(*root)->left, t);
    return;
  } else if ((*root)->right == NULL || t->lo > (*root)->right->lo) {
    insert(&(*root)->right, t);
    return;
  } else {
    t->left = (*root)->left;
    t->right = (*root)->right;
    insert(&(*root)->right, *root);
    root = &t;
  }
}
#include "sf2.c"
int main() {
  sf2_load_from_mem()

      TreeNode *t = newTree(1, 55, NULL);

  assert(t->left == NULL);
  TreeNode *start = 0;

  return 1;
}