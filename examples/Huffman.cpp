/**
 * @file   Huffman.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Build a Huffman tree
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <Board.h>
#include <map>
#include <vector>

using namespace LibBoard;

struct Node {
  std::string symbol;
  double probability;
  std::string codeword;
  Node * left;
  Node * right;
  Node(const std::string & symbol, double probability, Node * left = nullptr, Node * right = nullptr) //
      : symbol(symbol), probability(probability), left(left), right(right)
  {
  }
  ~Node()
  {
    delete left;
    delete right;
  }
  int height() const
  {
    int h = 1;
    if (left) {
      h = 1 + left->height();
    }
    if (right) {
      h = std::max(h, 1 + right->height());
    }
    return h;
  }
};

void extract_probabilities(const Node * tree, std::map<std::string, double> & m)
{
  if (!tree) {
    return;
  }
  extract_probabilities(tree->left, m);
  extract_probabilities(tree->right, m);
  if (!tree->left && !tree->right) {
    m[tree->symbol] = tree->probability;
  }
}

void extract_leaves(const Node * tree, std::vector<Node> & n)
{
  if (!tree) {
    return;
  }
  extract_leaves(tree->left, n);
  extract_leaves(tree->right, n);
  if (!tree->right && !tree->left) {
    n.push_back(Node{tree->symbol, tree->probability});
  }
}

Node * clone(Node * node)
{
  if (!node) {
    return nullptr;
  }
  return new Node(node->symbol, node->probability, clone(node->left), clone(node->right));
}

void fill_probabilities(Node * tree, const std::map<std::string, double> & m)
{
  if (!tree) {
    return;
  }
  if (!tree->left && !tree->right) {
    tree->probability = m.at(tree->symbol);
  } else {
    fill_probabilities(tree->left, m);
    fill_probabilities(tree->right, m);
  }
}

void fill_probabilities(Node * tree, double probability)
{
  if (!tree) {
    return;
  }
  if (tree->left && tree->right) {
    fill_probabilities(tree->left, probability * 0.5);
    fill_probabilities(tree->right, probability * 0.5);
  } else {
    tree->probability = probability;
  }
}

Group LeaveBox(const Node & node)
{
  char text[128];
  sprintf(text, "%.3g", node.probability);
  Group top = boardFontText(Point(), node.symbol, 10, Color::Blue);
  Group bottom = boardFontText(Point(), std::string(text), 10, Color::Blue);
  Group codeword = boardFontText(Point(), std::string(node.codeword), 5, Color::Black);
  top.append(bottom, Direction::Bottom, Alignment::Center, 5);
  top.append(codeword, Direction::Bottom, Alignment::Center, 5);
  return framed(top, 5.0, Color::Blue, Color::Null, 1.0);
}

struct NodeGreaterThan {
  bool operator()(const Node * a, const Node * b)
  {                                             //
    return (a->probability > b->probability) || //
           ((a->probability == b->probability) && (a->height() > b->height()));
  }
};

Node * HuffmanTree(const std::vector<Node> & nodes)
{
  if (nodes.empty()) {
    return nullptr;
  }
  std::priority_queue<Node *, std::vector<Node *>, NodeGreaterThan> nodeQueue;
  for (const Node & node : nodes) {
    nodeQueue.push(new Node{node.symbol, node.probability});
  }
  while (nodeQueue.size() != 1) {
    Node * left = nodeQueue.top();
    nodeQueue.pop();
    Node * right = nodeQueue.top();
    nodeQueue.pop();
    if (nodeQueue.size()) {
      if ((nodeQueue.top()->probability == left->probability) || //
          (nodeQueue.top()->probability == right->probability)) {
        std::cerr << "Warning: Non unique tree because of probability " //
                  << nodeQueue.top()->probability << std::endl;
      }
    }
    nodeQueue.push(new Node{"", left->probability + right->probability, left, right});
  }
  return nodeQueue.top();
}

Node * canonicalized(Node * tree)
{
  std::map<std::string, double> p;
  Node * c = clone(tree);
  extract_probabilities(c, p);
  fill_probabilities(c, 1.0);
  std::vector<Node> n;
  extract_leaves(c, n);
  delete c;
  Node * result = HuffmanTree(n);
  fill_probabilities(result, p);
  return result;
}

void fill_codeword(const std::string codeword, Node * tree)
{
  if (!tree) {
    return;
  }
  tree->codeword = codeword;
  if (tree->left && tree->right) {
    fill_codeword(codeword + "0", tree->left);
    fill_codeword(codeword + "1", tree->right);
  }
}

Group merge(Group left, Group right)
{
  const float margin = 5;
  Rect bboxLeft = left.boundingBox(LineWidthFlag::IgnoreLineWidth);
  Rect bboxRight = right.boundingBox(LineWidthFlag::IgnoreLineWidth);
  const double fullWidth = bboxLeft.width + margin + bboxRight.width;
  Group g;
  left.moveCenter(bboxLeft.width * 0.5, bboxLeft.height * -0.5);
  right.moveCenter(bboxLeft.width + margin + 0.5 * bboxRight.width, bboxRight.height * -0.5);

  Polyline polyline(
      {
          Point(bboxLeft.width * 0.5, 0.0),                           //
          Point(fullWidth * 0.5, margin * 3),                         //
          Point(bboxLeft.width + margin + 0.5 * bboxRight.width, 0.0) //
      },
      Path::Open, Color::Green, Color::Null, 1.0, LineStyle::SolidStyle, LineCap::RoundCap);

  g << left << right << polyline;
  Group labelLeft = boardFontText(Point(), std::string("0"), 3, Color::Black);
  Group box;
  box << rectangle(labelLeft.bbox(LineWidthFlag::UseLineWidth).growed(2), Color::Green, Color::White);
  box << labelLeft.moveCenter(labelLeft.center());
  box.moveCenter(mix(polyline[0], polyline[1], 0.5));
  g << box;
  Group labelRight = boardFontText(Point(), std::string("1"), 3, Color::Black);
  box.clear();
  box << rectangle(labelRight.bbox(LineWidthFlag::UseLineWidth).growed(2), Color::Green, Color::White);
  box << labelRight.moveCenter(labelRight.center());
  box.moveCenter(mix(polyline[1], polyline[2], 0.5));
  g << box;

  return g;
}

Group HuffmanTree(Node * node)
{
  if (!node->left && !node->right) {
    return LeaveBox(*node);
  }
  Group left;
  left << HuffmanTree(node->left);
  Group right;
  right << HuffmanTree(node->right);
  return merge(left, right);
}

int main(int, char *[])
{
  Board board;
  Style::setDefaultLineWidth(0.5);

  std::vector<Node> nodes = {
      {"X", 0.73},  //
      {"N", 0.08},  //
      {"H", 0.09},  //
      {"W", 0.04},  //
      {"P", 0.03},  //
      {"E", 0.015}, //
      {"Z", 0.015}  //
  };

  Node * tree = HuffmanTree(nodes);

  Node * c = canonicalized(tree);
  fill_codeword(std::string(""), c);
  board << HuffmanTree(c);

  delete tree;
  delete c;

  board.saveSVG("Huffman.svg");
  // system("display Huffman.svg");
}
