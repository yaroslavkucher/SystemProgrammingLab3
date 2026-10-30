#include <iostream>
#include <fstream>
#include "ast.hpp"

extern int yyparse();
extern ASTNode* root;

int main() {
    std::cout << "Enter C-like statements (e.g., x = 5 + 2;) and press Ctrl+Z (Windows) or Ctrl+D (Linux) to finish:" << std::endl;
    
    if (yyparse() == 0 && root != nullptr) {
        std::ofstream jsonFile("ast.json");
        jsonFile << root->toJson() << std::endl;
        jsonFile.close();
        std::cout << "[+] AST successfully saved to ast.json\n";
        
        std::ofstream pyFile("output.py");
        std::string pyCode = root->generatePython();
        pyFile << pyCode << "\n";
        pyFile.close();
        std::cout << "[+] Target Python code generated in output.py\n";
        
        int result = root->eval();
        std::cout << "[+] Semantics eval result: " << result << std::endl;

        delete root;
    } else {
        std::cerr << "Parsing failed." << std::endl;
    }
    
    return 0;
}