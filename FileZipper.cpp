#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <string>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

// Node structure for Huffman tree
class Node {
public:
    char data;
    unsigned freq;
    Node* left;
    Node* right;

    Node(char data, unsigned freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

// Comparison function for priority queue
class Compare {
public:
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

class HuffmanCoding {
private:
    Node* root;
    unordered_map<char, string> huffmanCode;
    int bitCount;
    char byte;

    // Helper function to generate Huffman codes
    void generateCodes(Node* node, const string& str) {
        if (!node) return;

        if (node->data != '\0') {
            huffmanCode[node->data] = str;
        }

        generateCodes(node->left, str + "0");
        generateCodes(node->right, str + "1");
    }

    // Helper function to write bits to file
    void writeBits(ofstream& outFile, const string& bits) {
        for (char bit : bits) {
            byte = (byte << 1) | (bit - '0');
            bitCount++;

            if (bitCount == 8) {
                outFile.put(byte);
                bitCount = 0;
                byte = 0;
            }
        }
    }

    // Helper function to read bits from file
    string readBits(ifstream& inFile) {
        string bits;
        char byte;
        while (inFile.get(byte)) {
            for (int i = 7; i >= 0; --i) {
                bits += ((byte >> i) & 1) ? '1' : '0';
            }
        }
        return bits;
    }

public:
    HuffmanCoding() : root(nullptr), bitCount(0), byte(0) {}

    // Build Huffman tree from character frequencies
    void buildTree(const unordered_map<char, int>& frequency) {
        priority_queue<Node*, vector<Node*>, Compare> pq;

        for (auto& pair : frequency) {
            pq.push(new Node(pair.first, pair.second));
        }

        while (pq.size() > 1) {
            Node* left = pq.top(); pq.pop();
            Node* right = pq.top(); pq.pop();

            Node* internal = new Node('\0', left->freq + right->freq);
            internal->left = left;
            internal->right = right;
            pq.push(internal);
        }

        if (!pq.empty()) {
            root = pq.top();
        } else {
            root = nullptr;
        }
    }

    // Generate Huffman codes
    void generateHuffmanCodes() {
        if (root) {
            generateCodes(root, "");
        } else {
            cerr << "Error: Huffman tree is empty. Cannot generate codes.\n";
        }
    }

    // Compress a file
    void compressFile(const string& inputFile, const string& outputFile) {
        ifstream file(inputFile, ios::binary);
        ofstream outFile(outputFile, ios::binary);

        char ch;
        while (file.get(ch)) {
            string code = huffmanCode[ch];
            writeBits(outFile, code);
        }

        // Write remaining bits if any
        if (bitCount > 0) {
            byte <<= (8 - bitCount);
            outFile.put(byte);
        }

        file.close();
        outFile.close();
    }

   

    // Get the Huffman codes
    const unordered_map<char, string>& getHuffmanCodes() const {
        return huffmanCode;
    }
};

void menu() {
    HuffmanCoding huffman;
    int choice;

    do {
        cout << "\nHuffman Coding Menu:\n";
        cout << "1. Compress a file\n";
        cout << "2. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            string inputPath, outputPath;
            cout << "Enter the path of the file to compress: ";
            cin >> inputPath;

            if (!fs::exists(inputPath)) {
                cout << "File does not exist. Please try again.\n";
                break;
            }

            cout << "Enter the path for the compressed output file: ";
            cin >> outputPath;

            unordered_map<char, int> frequency;
            ifstream file(inputPath, ios::binary);
            char ch;
            while (file.get(ch)) {
                frequency[ch]++;
            }
            file.close();

            huffman.buildTree(frequency);
            huffman.generateHuffmanCodes();
            huffman.compressFile(inputPath, outputPath);

            cout << "File compressed successfully!\n";
            break;
        }
        case 2:
            cout << "Exiting program.\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 3);
}

int main() {
    menu();
    return 0;
}
