#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <sstream>
#include <ctime>

#include <GL/glew.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <Windows.h>
#include <ShObjIdl.h> 

using namespace std;
namespace fs = std::filesystem;

// Node structure for Huffman tree
class Node {
public:
    char data;
    unsigned freq;
    Node* left;
    Node* right;

    Node(char data, unsigned freq) : data(data), freq(freq), left(NULL), right(NULL) {}
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
    string originalFileExtension;

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

    void cleanup(Node* node) {
        if (node) {
            cleanup(node->left);
            cleanup(node->right);
            delete node;
        }
    }

public:
    HuffmanCoding() : root(NULL), bitCount(0), byte(0) {}

    ~HuffmanCoding() {
        cleanup(root);
    }

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
            root = NULL;
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
    bool compressFile(const string& inputFile, const string& outputFile) {
        try {
            // Store original file extension
            fs::path inputPath(inputFile);
            originalFileExtension = inputPath.extension().string();
            
            ifstream inFile(inputFile, ios::binary);
            ofstream outFile(outputFile, ios::binary);

            // Write file extension to header
            size_t extLen = originalFileExtension.length();
            outFile.write(reinterpret_cast<const char*>(&extLen), sizeof(extLen));
            outFile.write(originalFileExtension.c_str(), extLen);

            // First, calculate frequencies
            unordered_map<char, int> frequency;
            char ch;
            while (inFile.get(ch)) {
                frequency[ch]++;
            }
            inFile.clear();
            inFile.seekg(0);

            // Write frequency table size
            size_t freqSize = frequency.size();
            outFile.write(reinterpret_cast<const char*>(&freqSize), sizeof(freqSize));

            // Write frequency table
            for (const auto& pair : frequency) {
                outFile.write(&pair.first, sizeof(char));
                outFile.write(reinterpret_cast<const char*>(&pair.second), sizeof(int));
            }

            buildTree(frequency);
            generateHuffmanCodes();

            // Write compressed data
            while (inFile.get(ch)) {
                if (huffmanCode.find(ch) == huffmanCode.end()) {
                    return false;
                }
                string code = huffmanCode[ch];
                writeBits(outFile, code);
            }

            // Write remaining bits
            if (bitCount > 0) {
                byte <<= (8 - bitCount);
                outFile.put(byte);
            }

            inFile.close();
            outFile.close();
            return true;
        }
        catch (const std::exception& e) {
            cerr << "Compression error: " << e.what() << endl;
            return false;
        }
    }

    // Decompress a file
    bool decompressFile(const string& inputFile, const string& outputFile) {
        try {
            ifstream inFile(inputFile, ios::binary);
            ofstream outFile(outputFile);

            // Read frequency table
            size_t freqSize;
            inFile.read(reinterpret_cast<char*>(&freqSize), sizeof(freqSize));

            unordered_map<char, int> frequency;
            for (size_t i = 0; i < freqSize; i++) {
                char ch;
                int freq;
                inFile.read(&ch, sizeof(char));
                inFile.read(reinterpret_cast<char*>(&freq), sizeof(int));
                frequency[ch] = freq;
            }

            // Rebuild Huffman tree
            buildTree(frequency);
            generateHuffmanCodes();

            // Read compressed data
            string bits = readBits(inFile);
            Node* current = root;
            
            for (char bit : bits) {
                if (!current) break;
                
                current = (bit == '0') ? current->left : current->right;

                if (current && current->data != '\0') {
                    outFile.put(current->data);
                    current = root;
                }
            }

            inFile.close();
            outFile.close();
            return true;
        }
        catch (const std::exception& e) {
            cerr << "Decompression error: " << e.what() << endl;
            return false;
        }
    }

    // Get the Huffman codes
    const unordered_map<char, string>& getHuffmanCodes() const {
        return huffmanCode;
    }
};

// File dialog helper function
string openFileDialog(bool save = false) {
    string filename;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        IFileDialog* pFileDialog;
        if (save) {
            hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileDialog));
        } else {
            hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileDialog));
        }

        if (SUCCEEDED(hr)) {
            hr = pFileDialog->Show(NULL);
            if (SUCCEEDED(hr)) {
                IShellItem* pItem;
                hr = pFileDialog->GetResult(&pItem);
                if (SUCCEEDED(hr)) {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    if (SUCCEEDED(hr)) {
                        wstring ws(pszFilePath);
                        filename = string(ws.begin(), ws.end());
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileDialog->Release();
        }
        CoUninitialize();
    }
    return filename;
}

// Add this structure after existing classes
struct FileRecord {
    string originalPath;
    string compressedPath;
    string decompressPath;
    string fileType;
    time_t timestamp;
};

class FileTracker {
private:
    const string logFile = "compression_history.txt";
    
    string getCurrentTimestamp() {
        time_t now = time(nullptr);
        char buffer[26];
        ctime_s(buffer, sizeof(buffer), &now);
        string timestamp(buffer);
        return timestamp.substr(0, timestamp.length() - 1); // Remove newline
    }

public:
    void saveRecord(const FileRecord& record) {
        ofstream out(logFile, ios::app);
        out << "Original: " << record.originalPath << "\n"
            << "Compressed: " << record.compressedPath << "\n"
            << "Decompress: " << record.decompressPath << "\n"
            << "Type: " << record.fileType << "\n"
            << "Time: " << getCurrentTimestamp() << "\n\n";
    }

    string generateCompressedPath(const string& inputPath) {
        fs::path path(inputPath);
        string stem = path.stem().string();
        return stem + "_compressed.bin";
    }

    string generateDecompressPath(const string& inputPath) {
        fs::path path(inputPath);
        string originalExt = ".txt"; // Default to .txt
        
        // Read original extension from compressed file header
        ifstream in(inputPath, ios::binary);
        if (in) {
            size_t extLen;
            in.read(reinterpret_cast<char*>(&extLen), sizeof(extLen));
            if (extLen < 10) { // Sanity check
                string ext(extLen, '\0');
                in.read(&ext[0], extLen);
                originalExt = ext;
            }
            in.close();
        }
        
        string stem = path.stem().string();
        return stem + "_decompressed" + originalExt;
    }
};

void guiMenu() {
    if (!glfwInit())
        return;

    GLFWwindow* window = glfwCreateWindow(800, 600, "File Zipper", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    HuffmanCoding huffman;
    FileTracker fileTracker;
    char inputPath[256] = "";
    char outputPath[256] = "";
    bool showSuccess = false;
    string statusMessage;
    bool showError = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Center window
        ImGui::SetNextWindowPos(ImVec2(400, 300), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(600, 400));

        ImGui::Begin("File Zipper", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        
        ImGui::Text("Input File:");
        ImGui::InputText("##input", inputPath, sizeof(inputPath));
        ImGui::SameLine();
        if (ImGui::Button("Browse Input")) {
            string path = openFileDialog();
            if (!path.empty()) {
                strncpy(inputPath, path.c_str(), sizeof(inputPath) - 1);
            }
        }

        ImGui::Text("Output File:");
        ImGui::InputText("##output", outputPath, sizeof(outputPath));
        ImGui::SameLine();
        if (ImGui::Button("Browse Output")) {
            string path = openFileDialog(true);
            if (!path.empty()) {
                strncpy(outputPath, path.c_str(), sizeof(outputPath) - 1);
            }
        }

        if (ImGui::Button("Compress", ImVec2(120, 0))) {
            if (strlen(inputPath) > 0) {
                if (fs::exists(inputPath)) {
                    string compressedPath = fileTracker.generateCompressedPath(inputPath);
                    if (huffman.compressFile(inputPath, compressedPath)) {
                        FileRecord record;
                        record.originalPath = inputPath;
                        record.compressedPath = compressedPath;
                        record.fileType = fs::path(inputPath).extension().string();
                        fileTracker.saveRecord(record);
                        
                        showSuccess = true;
                        showError = false;
                        statusMessage = "File compressed successfully to: " + compressedPath;
                        strncpy(outputPath, compressedPath.c_str(), sizeof(outputPath) - 1);
                    } else {
                        showError = true;
                        showSuccess = false;
                        statusMessage = "Compression failed!";
                    }
                } else {
                    showError = true;
                    statusMessage = "Input file does not exist!";
                }
            } else {
                showError = true;
                statusMessage = "Please select input file!";
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Decompress", ImVec2(120, 0))) {
            if (strlen(inputPath) > 0) {
                if (fs::exists(inputPath)) {
                    string decompressPath = fileTracker.generateDecompressPath(inputPath);
                    if (huffman.decompressFile(inputPath, decompressPath)) {
                        FileRecord record;
                        record.compressedPath = inputPath;
                        record.decompressPath = decompressPath;
                        fileTracker.saveRecord(record);
                        
                        showSuccess = true;
                        showError = false;
                        statusMessage = "File decompressed successfully to: " + decompressPath;
                        strncpy(outputPath, decompressPath.c_str(), sizeof(outputPath) - 1);
                    } else {
                        showError = true;
                        showSuccess = false;
                        statusMessage = "Decompression failed!";
                    }
                } else {
                    showError = true;
                    statusMessage = "Input file does not exist!";
                }
            } else {
                showError = true;
                statusMessage = "Please select input file!";
            }
        }

        if (showSuccess || showError) {
            ImGui::TextColored(
                showSuccess ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                "%s", statusMessage.c_str()
            );
        }

        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main() {
    guiMenu();
    return 0;
}
