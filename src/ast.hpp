#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>

inline std::map<std::string, int> variables;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::string toJson() const = 0;
    virtual std::string generatePython(int indent = 0) const = 0;
    virtual int eval() const = 0;
};

class NumberNode : public ASTNode {
public:
    int value;
    NumberNode(int val) : value(val) {}

    std::string toJson() const override {
        return "{\"name\": \"" + std::to_string(value) + "\"}";
    }

    std::string generatePython(int indent = 0) const override {
        return std::to_string(value);
    }

    int eval() const override { return value; }
};

class VarNode : public ASTNode {
public:
    std::string name;
    VarNode(const std::string& name) : name(name) {}

    std::string toJson() const override {
        return "{\"name\": \"" + name + "\"}";
    }

    std::string generatePython(int indent = 0) const override {
        return name;
    }

    int eval() const override { return variables[name]; }
};

class BinOpNode : public ASTNode {
public:
    std::string op;
    ASTNode* left;
    ASTNode* right;
    BinOpNode(const std::string op, ASTNode* left, ASTNode* right) : op(op), left(left), right(right) {}

    ~BinOpNode() {
        delete left;
        delete right;
    }

    std::string toJson() const override {
        std::string res = "{\"name\": \"";
        res += op;
        res += "\", \"children\": [";
        res += left->toJson() + ", ";
        res += right->toJson();
        res += "]}";
        return res;
    }

    std::string generatePython(int indent = 0) const override {
        return left->generatePython(0) + " " + op + " " + right->generatePython(0);
    }

    int eval() const override {
        if (op == "+") return left->eval() + right->eval();
        if (op == "-") return left->eval() - right->eval();
        if (op == "*") return left->eval() * right->eval();
        if (op == "/") return left->eval() / right->eval();
        
        if (op == "<") return left->eval() < right->eval();
        if (op == ">") return left->eval() > right->eval();
        if (op == "<=") return left->eval() <= right->eval();
        if (op == ">=") return left->eval() >= right->eval();
        if (op == "==") return left->eval() == right->eval();
        if (op == "!=") return left->eval() != right->eval();
        
        if (op == "=")
        {
            int val = right->eval();
            variables[static_cast<VarNode*>(left)->name] = val;
            return val;
        }
        return 0;
    }
};

class PrintNode : public ASTNode {
public:
    ASTNode* expr;
    
    PrintNode(ASTNode* e) : expr(e) {}
    ~PrintNode() { delete expr; }
    
    std::string toJson() const override {
        return "{\"name\": \"Print\", \"children\": [" + expr->toJson() + "]}";
    }
    
    std::string generatePython(int indent = 0) const override {
        return "print(" + expr->generatePython(0) + ")";
    }
    
    int eval() const override {
        int val = expr->eval();
        std::cout << ">>> Print output: " << val << std::endl;
        return val;
    }
};

class BlockNode : public ASTNode {
public:
    std::vector<ASTNode*> statements;
    ~BlockNode() { for (auto stmt : statements) delete stmt; }
    
    std::string toJson() const override {
        std::string res = "{\"name\": \"Block\", \"children\": [";
        for (size_t i = 0; i < statements.size(); ++i) {
            res += statements[i]->toJson() + (i < statements.size() - 1 ? ", " : "");
        }
        return res + "]}";
    }
    
    std::string generatePython(int indent = 0) const override {
        std::string res = "";
        for (auto stmt : statements) {
            res += std::string(indent, ' ') + stmt->generatePython(indent) + "\n";
        }
        return res.empty() ? std::string(indent, ' ') + "pass\n" : res;
    }
    
    int eval() const override {
        int last = 0;
        for (auto stmt : statements) last = stmt->eval();
        return last;
    }
};

class WhileNode : public ASTNode {
public:
    ASTNode* condition;
    ASTNode* body;
    WhileNode(ASTNode* cond, ASTNode* b) : condition(cond), body(b) {}
    ~WhileNode() { delete condition; delete body; }
    
    std::string toJson() const override {
        return "{\"name\": \"While\", \"children\": [" + condition->toJson() + ", " + body->toJson() + "]}";
    }
    
    std::string generatePython(int indent = 0) const override {
        return "while " + condition->generatePython(0) + ":\n" + body->generatePython(indent + 4);
    }
    
    int eval() const override {
        while (condition->eval()) { body->eval(); }
        return 0;
    }
};

class IfNode : public ASTNode {
public:
    ASTNode* condition;
    ASTNode* ifBody;
    ASTNode* elseBody;
    
    IfNode(ASTNode* cond, ASTNode* ifB, ASTNode* elseB = nullptr) 
        : condition(cond), ifBody(ifB), elseBody(elseB) {}
        
    ~IfNode() { 
        delete condition; 
        delete ifBody; 
        delete elseBody; 
    }
    
    std::string toJson() const override {
        std::string res = "{\"name\": \"If\", \"children\": [" + condition->toJson() + ", " + ifBody->toJson();
        if (elseBody) res += ", " + elseBody->toJson();
        res += "]}";
        return res;
    }
    
    std::string generatePython(int indent = 0) const override {
        std::string res = "if " + condition->generatePython(0) + ":\n" + ifBody->generatePython(indent + 4);
        if (elseBody) {
            res += std::string(indent, ' ') + "else:\n" + elseBody->generatePython(indent + 4);
        }
        return res;
    }
    
    int eval() const override {
        if (condition->eval()) {
            return ifBody->eval();
        } else if (elseBody) {
            return elseBody->eval();
        }
        return 0;
    }
};