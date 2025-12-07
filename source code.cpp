#include<iostream>
#include<string>
#include<cctype>
#include<cmath>
#include<sstream>
#include<map>
#include<vector>
#include<functional>

using namespace std;

template<typename T>
class MyStack{
    private:
    struct Node{
        T data;
        Node* next;
        Node(T value) : data(value), next(nullptr){}
};
    Node* topNode;

    public:

    MyStack():topNode(nullptr){}

    ~MyStack(){ 
    while (!isEmpty()){
        pop();
    }
    }

    void push(T value){
        Node* newNode = new Node(value);
        newNode ->next  =  topNode;
        topNode = newNode;
    }

    void pop(){
        if (isEmpty()){
            cout<<"栈为空"<<endl;
            return;
        }
        Node* temp = topNode;
        topNode = topNode -> next;
        delete temp;
    }

    T top()const{
        if (isEmpty()){
            throw runtime_error("栈为空");
        }
        return topNode -> data;
    }

    bool isEmpty()const{
        return topNode == nullptr;
    }

    size_t size()const{
        size_t count = 0 ;
        Node* current = topNode;

        while (current !=  nullptr){
            count++;
            current = current -> next;
        }
        return count;

    }
};


map<string, double> variables;



// 数学函数映射
map<string, function<double(double)>> mathFunctions = {
    {"sin", [](double x) { return sin(x); }},
    {"cos", [](double x) { return cos(x); }},
    {"tan", [](double x) { return tan(x); }},
    {"log", [](double x) { return log(x); }},
    {"ln",  [](double x) { return log(x); }},
    {"log10", [](double x) { return log10(x); }},
    {"sqrt", [](double x) { return sqrt(x); }},
    {"abs",  [](double x) { return abs(x); }},
    {"ceil", [](double x) { return ceil(x); }},
    {"floor",[](double x) { return floor(x); }}
};


// 检查是否为数学函数
bool isFunction(const string& expr, int& start, int end) {
    // 向前查找直到遇到非字母字符
    int funcStart = end - 1;
    while (funcStart >= 0 && isalpha(expr[funcStart])) {
        funcStart--;
    }
    funcStart++;
    
    if (funcStart < end) {
        string funcName = expr.substr(funcStart, end - funcStart);
        start = funcStart;
        return mathFunctions.find(funcName) != mathFunctions.end();
    }
    return false;
}

//获取运算符优先级
int getPriority(char op){
    switch(op){
        case '=' : return 1;
        case '(' : case ')' : return 2;
        case '+' : case '-' : return 3;
        case '*' : case '/' : case '%' :  return 4;
        case '^' : return 5;
        default : return 0;
    }
}
//判断是否为单目运算符
bool isUnaryOpertartor(const string& expr, int pos){
    if (pos == 0)return true;
    int p = pos - 1;
    while (p >= 0 && expr[p] == ' ') p--;
    if (p < 0) return true; // 已到开头
    char prev = expr[p];
    /*char prev = expr[pos - 1];*/
    return (prev == '(' || prev == '+' || prev == '-' || prev == '*' || prev == '/' || prev == '%' || prev == '^' || prev == '=');
}
//中缀转后缀
bool infixToPostfix(const string& infix , string& postfix){
    MyStack<char> operators;
    MyStack<string> functions;
    postfix= "";
    string number = "";
    string identifier = "";

    for (int i=0;i<infix.length();i++){
        char ch = infix[i]; 
        
        if (ch == ' ') {
            if (!number.empty()) {
                postfix += number + " ";
                number = "";
            }
            continue;
            if (!identifier.empty()) {
                if (variables.find(identifier) != variables.end()) {
                    postfix += to_string(variables[identifier]) + " ";
                } else {
                    postfix += identifier + " ";
                }
                identifier = "";
            }
            continue;
        }
        


        //对数字处理
        if(isdigit(ch) || ch=='.'){
            if (!identifier.empty()) {
                // 如果之前有标识符，先处理它
                if (variables.find(identifier) != variables.end()) {
                    postfix += to_string(variables[identifier]) + " ";
                } else {
                    postfix += identifier + " ";
                }
                identifier = "";
            }
            number += ch;
        }
        // 对字母处理（可能是变量或函数名）
        else if (isalpha(ch)) {
            if (!number.empty()) {
                postfix += number + " ";
                number = "";
            }
            identifier += ch;
        }
        else{
            if(!number.empty()){
                postfix += number + " ";
                number = "";
        }    
            if (!identifier.empty()) {
                // 检查是否为函数
                if (mathFunctions.find(identifier) != mathFunctions.end()) {
                    functions.push(identifier);
                } else if (variables.find(identifier) != variables.end()) {
                    postfix += to_string(variables[identifier]) + " ";
                } else {
                    postfix += identifier + " ";
                }
                identifier = "";
            }
        
        //运算符处理
        if(ch == '('){
            operators.push(ch);
            // 如果前面有函数，将函数也压入栈
                if (!functions.isEmpty()) {
                    operators.push('@'); // 用@表示函数调用
                }
        }
        else if(ch == ')'){
            while(!operators.isEmpty() && operators.top() != '('){
                if (operators.top() == '@') {
                        // 函数调用
                        operators.pop();
                        if (!functions.isEmpty()) {
                            string funcName = functions.top();
                            functions.pop();
                            postfix += funcName + " ";
                        }
                    } else {
                        postfix += operators.top();
                        postfix += " ";
                        operators.pop();
                    }
            }
            if (operators.isEmpty()){
                cout<<"运算错误，括号不匹配"<<endl;
                return false;
            }
            operators.pop();//左括号弹出
        }
        else if (ch== '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '^' || ch == '=')
        {
            if ((ch == '-' || ch == '+') && isUnaryOpertartor(infix, i)){//单目变为双目
                postfix += "0 ";
            }
                while (!operators.isEmpty() &&  operators.top() != '(' && operators.top() != '@' && 
                       getPriority(operators.top()) >= getPriority(ch)){
                    if (operators.top() == '@') {
                        operators.pop();
                        if (!functions.isEmpty()) {
                            string funcName = functions.top();
                            functions.pop();
                            postfix += funcName + " ";
                        }
                    } else {
                        postfix += operators.top();
                        postfix += " ";
                        operators.pop();
                    }
                }
                operators.push(ch);
            
        }
        else{
            cout<<"运算错误，未知字符: "<< ch << "." <<endl;
            return false;

        }
    }
    }
    if(!number.empty()){
        postfix += number + " ";
    }
    if (!identifier.empty()) {
        if (variables.find(identifier) != variables.end()) {
            postfix += to_string(variables[identifier]) + " ";
        } else {
            postfix += identifier + " ";
        }
    }
    while(!operators.isEmpty()){
        if (operators.top() == '@') {
            operators.pop();
            if (!functions.isEmpty()) {
                string funcName = functions.top();
                functions.pop();
                postfix += funcName + " ";
            }
        } else if(operators.top() == '(' || operators.top() == ')'){
            cout<<"运算错误，括号不匹配"<<endl;
            return false;
        } else {
            postfix += operators.top() ;
            postfix += " ";
            operators.pop();
        }
        } 
        return true;

    }
//计算后缀表达式
double calculatePostfix(const string& postfix, bool& success){
    MyStack<double> operands; 
    string token;
    stringstream ss(postfix); //创建字符串流,将输入的后缀表达式字符串加载到字符串流中，便于后续逐个读取操作数和运算符

    success = true;
    bool hasOperator = false;

    while (ss >> token){
        if (mathFunctions.find(token) != mathFunctions.end()) {
            if(operands.size() < 1){
                cout<<"运算错误，缺少操作数"<<endl;
                success = false;
                return 0;
            }
            double a = operands.top(); operands.pop();
            double result = mathFunctions[token](a);
            operands.push(result);
            hasOperator = true;
        }
        else if (isdigit(token[0]) || (token.length() > 1 && token[0] == '-' && isdigit(token[1]))){
            operands.push(stod(token));
        }
        else{
            hasOperator = true;

            if(token[0] == '=') {
                if(operands.size() < 1){
                    cout<<"运算错误，缺少操作数"<<endl;
                    success = false;
                    return 0;
                }
                else if(operands.size() >= 2) {
                    double b = operands.top();operands.pop();
                    double a = operands.top();operands.pop();
                    double result = (a == b);
                    operands.push(result);
                }
            }
            else {
                if(operands.size() < 2){
                    cout<<"运算错误，缺少操作数"<<endl;
                    success = false;
                    return 0;
                   }
                double b = operands.top();operands.pop();
                double a = operands.top();operands.pop();
                double result;

                switch(token[0]){
                    case '+' : result = a + b; break;
                    case '-' : result = a - b; break;
                    case '*' : result = a * b; break;
                    case '/' : 
                        if(b == 0){
                            cout<<"运算错误，除数不能为0"<<endl;
                            success = false;
                            return 0;
                        }
                        result = a / b;
                    break;
                    case '%' : 
                        if (b == 0){
                            cout<<"运算错误，模运算除数不能为0"<<endl;
                            success = false;
                            return 0;
                        }
                
                        result = fmod(a,b);
                        break;
                    case '^' : result = pow(a,b); break;
                    default:
                        cout<<"运算错误，未知字符: "<< token[0] << "." <<endl;
                        success = false;
                        return 0;
                }

                operands.push(result);
            }
        }
    }

    if(!hasOperator && operands.size() != 1){
            cout<< "运算错误，缺少运算符"<<endl;
            success = false;
            return 0;
        }

    if(operands.size() != 1){
        cout << "无输入，出现错误"<< endl;
        success = false;
        return 0;
        }
        
    return operands.top();
}

// 处理变量赋值
bool handleAssignment(string& expression) {
    size_t equalPos = expression.find('=');
    if (equalPos != string::npos && equalPos > 0) {
        string varName = expression.substr(0, equalPos);
        // 去除空格
        string trimmedVar = "";
        for (char c : varName) {
            if (c != ' ') trimmedVar += c;
        }
        
        if (!trimmedVar.empty() && isalpha(trimmedVar[0])) {
            string expr = expression.substr(equalPos + 1);
            string postfix;
            if (infixToPostfix(expr, postfix)) {
                bool success;
                double result = calculatePostfix(postfix, success);
                if (success) {
                    variables[trimmedVar] = result;
                    cout << trimmedVar << " = " << result << endl;
                    return true;
                }
            }
        }
    }
    return false;
}
//显示帮助信息
void showHelp() {
    cout << "\n=== 计算器帮助 ===" << endl;
    cout << "支持的功能:" << endl;
    cout << "1. 基本运算: +, -, *, /, %, ^(幂运算), = (相等比较)" << endl;
    cout << "2. 数学函数: sin, cos, tan, log, ln, log10, sqrt, abs, ceil, floor" << endl;
    cout << "3. 变量赋值: x = 5, y = x + 3" << endl;
    cout << "4. 特殊命令:" << endl;
    cout << "   - exit: 退出程序" << endl;
    cout << "   - vars: 显示所有变量" << endl;
    cout << "   - help: 显示帮助信息" << endl;
    cout << "示例:" << endl;
    cout << "   2 + 3 * 4" << endl;
    cout << "   sin(3.14159/2)" << endl;
    cout << "   x = 10" << endl;
    cout << "   y = x * 2 + 5" << endl;
    cout << "==================" << endl << endl;

}
//显示变量列表
void showVariables() {
    cout << "\n=== 变量列表 ===" << endl;
    if (variables.empty()) {
        cout << "暂无变量" << endl;
    } else {
        for (const auto& pair : variables) {
            cout << pair.first << " = " << pair.second << endl;
        }
    }
    cout << "=================" << endl << endl;
}
int main()
{
    string expression;
    showHelp();
    cout<<"请输入一个中缀表达式（输入exit）退出："<<endl;

    while(getline(cin, expression)){
        if(expression == "exit"){
            cout<<"程序已退出"<<endl;
            break;
        }
        if(expression == "help") {
            showHelp();
            cout<<"请输入一个中缀表达式（输入exit退出）："<<endl;
            continue;
        }
        

        if (expression.empty()){
            cout<<"请输入一个中缀表达式（输入exit）退出："<<endl;
            continue;
        }
        if(expression == "vars") {
            showVariables();
            cout<<"请输入一个中缀表达式（输入exit退出）："<<endl;
            continue;
        }
        if (handleAssignment(expression)) {
            cout<<"请输入一个中缀表达式（输入exit退出）："<<endl;
            continue;
        }


        string postfix;

        if (infixToPostfix(expression, postfix)){
            cout<< "转换为后缀表达式为：" << postfix << endl;

            bool success;
            double result  = calculatePostfix(postfix, success);

            if (success){
                cout<<"计算结果为：" << result << endl;

            }
        }

        cout<<"请输入一个中缀表达式（输入exit）退出："<<endl;
    }
    system("pause");
    return 0;
	
}