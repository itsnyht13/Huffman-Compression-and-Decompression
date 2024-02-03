#include <iostream>
#include <fstream>
#define MAX 256
using namespace std;

// HUFFMANCODING - DECOMPRESSION PROGRAM
// BSCS-2A-M
// January 16, 2024

struct NodeData{
    char data;
    int frequency;
    NodeData* left;
    NodeData* right;
    NodeData(char c, int frequency) : data(c), frequency(frequency), left(NULL), right(NULL) {}
};

struct Node{
    NodeData* node;
    Node* next;
    Node(NodeData* bn) : node(bn), next(NULL) {}
};

class Huffman{
    private:
        Node* root = NULL;
        int freqs[MAX] = {};
        string huffmancode[MAX] = {};
    public:
        void buildHuffmanTree(int freqs[MAX]);
        void priority_enqueue(char data, int frequency);
        void huffmancode_generator(NodeData* node, string code);
        void readFrequency_Table(string file_name);
        void decodeFile(string file_name);
        void display();
};

int main(){
    Huffman huffmancoding;
    int choice;
    string file_name;

    do{
        system("cls");
        cout << "||============================================================||" << endl;
        cout << "||   WELCOME TO HUFFMAN FILE COMPRESSION AND DECOMPRESSION    ||" << endl;
        cout << "||------------------------------------------------------------||" << endl;
        cout << "||                  DECOMPRESSION PROGRAM                     ||" << endl;
        cout << "||------------------------------------------------------------||" << endl;
        cout << "|| \t\t  [1]. DECOMPRESS A FILE   \t\t      ||" << endl;
        cout << "|| \t\t  [2]. EXIT \t\t\t\t      ||" << endl;
        cout << "||============================================================||" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch(choice){
            case 1:
                cout << "NOTE: Enter just the file name without the -compressed.dat name (Ex. test)" << endl;
                cout << "Enter a File Name: ";
                cin >> file_name;
                huffmancoding.decodeFile(file_name);
                break;
            case 2:
                cout << "Thank you for using our Compression and Decompression Program ^_^" << endl;
                exit(0);
                break;
            default:
                cout << "Invalid Choice. Try Again" << endl;
        }
    }while(choice != 2);
    return 0;
}


// What this function does is recreate the huffman tree of the compressed file
void Huffman::buildHuffmanTree(int freqs[MAX]){
    for(int i = 0; i < MAX; i++){
        if(freqs[i] > 0){
            // cout << "Char: " << char(i) << " Frequency: " << freqs[i] << endl;
            priority_enqueue(char(i), freqs[i]);
        }
    }

    // Same sa priority_enqueue but cinocombine na natin yung two unique characters
    while(root->next != NULL){
        Node* left = root;
        Node* right = root->next;

        NodeData* combinedNode = new NodeData('$', left->node->frequency + right->node->frequency);
        combinedNode->left = left->node;
        combinedNode->right = right->node;
        root = right->next;

        Node* newNode = new Node(combinedNode);
        if(root == NULL || newNode->node->frequency < root->node->frequency){
            newNode->next = root;
            root = newNode;
        }
        else{
            Node* current = root;
            while(current->next && newNode->node->frequency >= current->next->node->frequency){
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
        }
    }
    
    huffmancode_generator(root->node, "");
}

// What this function does is priority queue the characters and frequency from smallest to largest
void Huffman::priority_enqueue(char data, int frequency){
    NodeData* node = new NodeData(data, frequency);
    Node* newNode = new Node(node);

    if(root == NULL || newNode->node->frequency < root->node->frequency){
        newNode->next = root;
        root = newNode;
    }
    else{
        Node* current = root;
        while(current->next && newNode->node->frequency >= current->next->node->frequency){
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

// A function that creates the huffmancode for the unique character
void Huffman::huffmancode_generator(NodeData* node, string code){
    if(node->left == NULL and node->right == NULL){
        huffmancode[node->data] = code;
        return;
    }

    if(node->left != NULL){
        huffmancode_generator(node->left, code + '0');
    }

    if(node->right != NULL){
        huffmancode_generator(node->right, code + '1');
    }
}

// A function that reads the compressed file and decode the file
void Huffman::decodeFile(string file_name){

    readFrequency_Table(file_name); // We call the readFrequency_Table here to restore the specific compressed file its unique character and their frequencies
    buildHuffmanTree(freqs); // Then we call the buildHuffmanTree function again to recreate the tree and use it as guide
    display();

    fstream file;
    file.open(file_name+"-compressed.dat", ios::binary | ios::in);
    
    if(!file){
        cout << "Error Opening File" << endl;
        system("pause");
        return;
    }
    else{

        fstream outputFile(file_name+"-decompressed.txt", ios::out);
        NodeData* current = root->node;
        char byte;
        bool flag = false;

        while (file.read(&byte, sizeof(byte))) {
            if (byte == '\n') {
                if(flag){
                    break;
                } 
                else{
                    flag = true;
                }
            }
            else{
                flag = false;
            }
        }

        while(file.read(&byte, sizeof(byte))){
            for (int i = 7; i >= 0; i--) {

                char bit = (byte >> i) & 1;

                if (bit == 0) {
                    current = current->left;
                }
                else {
                    current = current->right;
                }

                if (current->left == nullptr && current->right == nullptr) {
                    outputFile << current->data; // Write the current symbol here in the outputFile
                    current = root->node;  // Reset to the root for the next symbol
                }
            }
        }
        outputFile.close();
    }

    file.close();

    cout << endl;
    cout << "====================================" << endl;
    cout << "|| FILE HAS SUCCESSFULLY DECODED  ||" << endl;
    cout << "====================================" << endl;

    system("pause");
}


// A function the reads the frequency table of the compressed file
void Huffman::readFrequency_Table(string file_name) {
    fstream file;
    file.open(file_name+"-compressed.dat", ios::in);

    if (!file) {
        cout << "File cannot be found" << endl;
        system("pause");
        return;
    }
    else {
        string unique_charac;
        int freq;

        while (file >> unique_charac >> freq) {
            if (unique_charac == "space") {
                unique_charac = " ";  // Change "SP" to space character
            }
            else if(unique_charac == "newline"){
                unique_charac = "\n"; // newline to enter character or \n
            }
            else if(unique_charac == "indent"){
                unique_charac = "\t";
            }
            char character = unique_charac[0];  // Extract the first character from the string
            freqs[character] = freq;
        }
    }
    file.close();
}


// A function that displays the character, frequencies and huffmancode
void Huffman::display(){
        cout << "\tASCII \tCHAR \tFREQ \tHUFFMAN CODE" << endl;
        for (int i = 0; i < MAX; ++i) {
            if (freqs[i] > 0) {
                cout << "\t" << i << " \t" << (char)i << " \t" << freqs[i] << " \t" << huffmancode[i] << endl;
            }
        }
}