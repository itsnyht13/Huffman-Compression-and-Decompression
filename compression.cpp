#include <iostream>
#include <fstream>
#define MAX 256
using namespace std;

// HUFFMANCODING - COMPRESSION PROGRAM
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

        // Compression Functions
        void readFile(string file_name);
        void buildHuffmanTree(int freqs[MAX]);
        void priority_enqueue(char data, int frequency);
        void huffmancode_generator(NodeData* node, string code);
        void encodeFile(string file_name);
        void save_frequency(string file_name);

        // Decompression Functions
        void decodeFile(string file_name);
        void readFrequency_Table(string file_name);
        
        // Debugging Function
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
        cout << "||                   COMPRESSION PROGRAM                      ||" << endl;
        cout << "||------------------------------------------------------------||" << endl;
        cout << "|| \t\t  [1]. COMPRESS A FILE   \t\t      ||" << endl;
        cout << "|| \t\t  [2]. EXIT \t\t\t\t      ||" << endl;
        cout << "||============================================================||" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch(choice){
            case 1:
                cout << "NOTE: Enter just the file name without the .txt name (Ex. test)" << endl;
                cout << "Enter a File Name: ";
                cin >> file_name;
                huffmancoding.readFile(file_name);
                huffmancoding.encodeFile(file_name);
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

void Huffman::readFile(string file_name){
    fstream file;
    file.open(file_name+".txt", ios::in);

    if(!file){
        cout << "Error Opening File" << endl;
        return;
    }
    else{
        char ch;
        while(file.get(ch)){
            if(ch > 0 && ch < MAX){
                freqs[ch]++;
            }
        }
        //Debugger for filereading
    }
    file.close();

    // We call the buildHuffmanTree to get the HuffmanCodes
    buildHuffmanTree(freqs);
}

void Huffman::buildHuffmanTree(int freqs[MAX]){
    for(int i = 0; i < MAX; i++){
        if(freqs[i] > 0){
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

// What this function does is isosortout niya yung mga binasa na characters and its frequency from smallest to largest kapag prinint
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

// This encodeFile function saves the frequency table and write the huffmancode in binary form
void Huffman::encodeFile(string file_name){
    fstream file;
    file.open(file_name+".txt", ios::in);

    if (!file) {
        cout << "Error Opening File" << endl;
        return;
    }
    else{
        fstream file_compressed(file_name+"-compressed.dat", ios::out | ios::binary);
        char ch;
        string compressed_data;

        // This for loop save the character and its frequency and store it in the compressed file
        for (int i = 0; i < MAX; i++){
            if (freqs[i] > 0){
                if (i == ' ') { // If space yung character then change natin tong character nato into SP
                    file_compressed << "space" << ' ' << freqs[i]; // Here we change the ' ' into SP as symbol since hindi nababasa ng program ang space or enter character as unique character
                }
                else if(i == '\n'){ // Same goes to newline or enter so instead we change it into "newline" as symbol
                    file_compressed << "newline" << ' ' << freqs[i];
                }
                else if(i == '\t'){
                    file_compressed << "indent" << ' ' << freqs[i];
                }
                else{
                    file_compressed << char(i) << ' ' << freqs[i];
                }
                file_compressed << endl;
            }
        }

        file_compressed << endl;

        while(file.get(ch)) {
            compressed_data += huffmancode[ch];
        }

        int padding = 8 - compressed_data.length() % 8;
        if (padding != 8) {
            compressed_data.append(padding, '0');
        }

        for (int i = 0; i < compressed_data.length(); i+= 8){
            char byte = 0;
            for (int j = 0; j < 8 && i + j < compressed_data.length(); j++) {
                byte = (byte << 1) | (compressed_data[i + j] - '0');
            }
            file_compressed.write(&byte, sizeof(byte));
        }

        // save_frequency(file_name); // We save the frequency here and create a frequency table
        file_compressed.close();
    }
    file.close();

    display();
    cout << endl;
    cout << "=======================================" << endl;
    cout << "|| FILE HAS SUCCESSFULLY COMPRESSED! ||" << endl;
    cout << "=======================================" << endl;
    system("pause");
}

void Huffman::display(){
        cout << "\tASCII \tCHAR \tFREQ \tHUFFMAN CODE" << endl;
        for (int i = 0; i < MAX; ++i) {
            if (freqs[i] > 0) {
                cout << "\t" << i << " \t" << (char)i << " \t" << freqs[i] << " \t" << huffmancode[i] << endl;
            }
        }
}