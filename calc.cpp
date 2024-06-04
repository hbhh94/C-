#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <map>
#include <regex>
using namespace std;

class Head
{
public:
    void head(string line)
    {
        cout << line << endl;
    }
};

class HexaConverter
{
public:
    string convertHexa(const string &exp)
    {
        smatch match;
        string result = exp;

        while (regex_search(result, match, regex("0x([0-9A-Fa-f]+)")))
        {
            string hexStr = match[1].str();
            int decimalValue = hexToDecimal(hexStr);
            string replacement = to_string(decimalValue);
            result = match.prefix().str() + replacement + match.suffix().str();
        }

        return result;
    }

private:
    int hexToDecimal(const string &hexStr)
    {
        int result = 0;
        int power = 1;

        for (int i = hexStr.size() - 1; i >= 0; --i)
        {
            if (isdigit(hexStr[i]))
            {
                result += (hexStr[i] - '0') * power;
            }
            else
            {
                result += (toupper(hexStr[i]) - 'A' + 10) * power;
            }

            power *= 16;
        }

        return result;
    }
};

class BinaryConverter
{
public:
    string convertBinary(const string &exp)
    {
        smatch match;
        string result = exp;

        while (regex_search(result, match, regex("([01]+)b")))
        {
            string binaryStr = match[1].str();
            string replacement = binaryToDecimal(binaryStr);
            result = match.prefix().str() + replacement + match.suffix().str();
        }

        return result;
    }

private:
    string binaryToDecimal(const string &binaryStr)
    {
        int result = 0;
        for (char bit : binaryStr)
        {
            result = result * 2 + (bit - '0');
        }
        return to_string(result);
    }
};

class SpaceRemover
{
public:
    string remove(const string &expression)
    {
        istringstream iss(expression);
        string result;
        string word;
        while (iss >> word)
        {
            result += word;
        }
        return result;
    }
};

class Parse
{
private:
    static map<string, double> variables;

public:
    void parseVariableDefinition(const string &line);
    string replaceVariableValue(string &exp);
};

map<string, double> Parse::variables;

void Parse::parseVariableDefinition(const string &line)
{
    string variableName;
    string variableValue;

    for (char token : line)
    {
        if (token == '=')
        {
            // Found the '=' character, assign the characters to the left as variableName
            variableName = line.substr(0, line.find('='));

            // Assign the characters to the right as variableValue
            variableValue = line.substr(line.find('=') + 1);

            // Insert the variable into the map
            variables[variableName] = stod(variableValue);

            // Break out of the loop since we've found the '='
            break;
        }
    }
}

string Parse::replaceVariableValue(string &exp)
{
    for (int i = 0; i < exp.length(); i++)
    {
        if (isalpha(exp[i]))
        {
            int index = i;
            string variablename;
            while (i < exp.size() && (isalpha(exp[i]) || isdigit(exp[i])))
            {
                variablename += exp[i];
                ++i;
            }
            --i;
            if (variablename == "sin" || variablename == "cos" || variablename == "tan")
            {
                continue;
            }
            else
            {
                if (variables.find(variablename) != variables.end())
                {
                    double value = variables[variablename];
                    exp.replace(index, variablename.length(), to_string(value));
                }
            }
        }
    }
    return exp;
}

class Calculater
{
public:
    double calculate(const string &expression)
    {
        stack<double> values;
        stack<char> operators;

        for (size_t i = 0; i < expression.size(); ++i)
        {
            char currentChar = expression[i];
            if (isdigit(currentChar))
            {
                string numberStr;
                while (i < expression.size() && (isdigit(expression[i]) || expression[i] == '.'))
                {
                    numberStr += expression[i];
                    ++i;
                }
                --i;
                values.push(stod(numberStr));
            }
            else if (currentChar == '-' && (i == 0 || expression[i - 1] == '(' || isOperator(expression[i - 1])))
            {
                string numberStr;
                while (i < expression.size() && (isdigit(expression[i]) || expression[i] == '.' || expression[i] == '-'))
                {
                    numberStr += expression[i];
                    ++i;
                }
                --i;
                values.push(stod(numberStr));
                continue;
            }
            else if (isOperator(currentChar))
            {
                // If the current character is an operator
                while (!operators.empty() && precedence(operators.top()) >= precedence(currentChar))
                {
                    applyOperator(values, operators.top());
                    operators.pop();
                }
                operators.push(currentChar);
            }
        }

        while (!operators.empty())
        {
            applyOperator(values, operators.top());
            operators.pop();
        }

        return values.top();
    }

private:
    bool isOperator(char c)
    {
        return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
    }

    int precedence(char op)
    {
        if (op == '^')
        {
            return 2;
        }
        else if (op == '*' || op == '/')
        {
            return 1;
        }
        else
        {
            return 0; // Addition and subtraction have the lowest precedence
        }
    }

    void applyOperator(stack<double> &values, char op)
    {
        // Binary operators
        double operand2 = values.top();
        values.pop();
        double operand1 = values.top();
        values.pop();

        switch (op)
        {
        case '+':
            values.push(operand1 + operand2);
            break;
        case '-':
            values.push(operand1 - operand2);
            break;
        case '*':
            values.push(operand1 * operand2);
            break;
        case '/':
            values.push(operand1 / operand2);
            break;
        case '^':
            values.push(pow(operand1, operand2));
            break;
        default:
            cerr << "Invalid operator: " << op << endl;
        }
    }
};

class ParenthesisReplacer
{
public:
    Calculater calc;
    string replaceParenthesisValues(const string &expression)
    {
        stack<size_t> openingParentheses;
        string modifiedExpression = expression;
        // Find matching parentheses pairs from left to right
        for (size_t i = 0; i < modifiedExpression.length(); ++i)
        {
            if (modifiedExpression[i] == '(')
            {
                openingParentheses.push(i);
            }
            else if (modifiedExpression[i] == ')')
            {
                if (!openingParentheses.empty())
                {
                    size_t openPos = openingParentheses.top();
                    openingParentheses.pop();

                    string insideParenthesis = modifiedExpression.substr(openPos + 1, i - openPos - 1);
                    string replacement = to_string(calc.calculate(insideParenthesis));

                    modifiedExpression.replace(openPos, i - openPos + 1, replacement); // Adjust the loop variable after replacement

                    // Adjust the current position and length based on the replacement
                    i = openPos + replacement.length() - 1;
                }
            }
        }

        return modifiedExpression;
    }
};

class Trigonometric
{
public:
    string trig(string exp)
    {
        string expression = exp;
        for (int i = 0; i < expression.length(); i++)
        {
            int index = 0;
            string deg = "";
            if (expression[i] == 's' && expression[i + 1] == 'i' && expression[i + 2] == 'n')
            {
                index = i;
                for (int j = index + 3; j < expression.length(); j++)
                {
                    if (isdigit(expression[j]))
                    {
                        deg = deg + expression[j];
                    }
                    else
                    {
                        break;
                    }
                }
                double x = stod(deg) * 3.14159 / 180;
                double rs = sin(x);
                string sub = "sin(" + deg + ")";
                expression.replace(index, sub.length(), to_string(rs));
            }
            else if (expression[i] == 'c' && expression[i + 1] == 'o' && expression[i + 2] == 's')
            {
                index = i;
                for (int j = index + 3; j < expression.length(); j++)
                {
                    if (isdigit(expression[j]))
                    {
                        deg = deg + expression[j];
                    }
                    else
                    {
                        break;
                    }
                }
                double x = stod(deg) * 3.14159 / 180;
                double rs = cos(x);
                string sub = "cos(" + deg + ")";
                expression.replace(index, sub.length(), to_string(rs));
            }
            else if (expression[i] == 't' && expression[i + 1] == 'a' && expression[i + 2] == 'n')
            {
                index = i;
                for (int j = index + 3; j < expression.length(); j++)
                {
                    if (isdigit(expression[j]))
                    {
                        deg = deg + expression[j];
                    }
                    else
                    {
                        break;
                    }
                }
                double x = stod(deg) * 3.14159 / 180;
                double rs = tan(x);
                string sub = "tan(" + deg + ")";
                expression.replace(index, sub.length(), to_string(rs));
            }
        }
        return expression;
    }
};

class Main
{
public:
    void evaluateFromFile(const string &filename)
    {
        ifstream inputFile(filename);

        if (!inputFile)
        {
            perror("Error opening file");
            return;
        }

        string line;
        while (getline(inputFile, line))
        {
            if (line.find("--") != string::npos)
            {
                Head hd;
                hd.head(line);
                continue;
            }
            else if (line.empty())
            {
                // Skip empty lines
                continue;
            }
            else if (line.find('=') != string::npos)
            {
                // Variable definition line
                SpaceRemover space;
                line = space.remove(line);
                Parse par;
                par.parseVariableDefinition(line);
            }
            else
            {
                SpaceRemover space;
                string cleanedExpression = space.remove(line);
                double result = evaluate(cleanedExpression);
                cout << result << endl;
            }
        }
        inputFile.close();
    }

private:
    BinaryConverter binary;
    HexaConverter hexa;
    Parse par;
    ParenthesisReplacer parenth;
    Trigonometric tri;
    Calculater calc;
    double evaluate(const string &expression)
    {
        string cleanedExpression = binary.convertBinary(expression);
        cleanedExpression = hexa.convertHexa(cleanedExpression);
        cleanedExpression = par.replaceVariableValue(cleanedExpression);
        cleanedExpression = parenth.replaceParenthesisValues(cleanedExpression);
        cleanedExpression = tri.trig(cleanedExpression);
        double result = calc.calculate(cleanedExpression);
        return result;
    }
};

int main()
{
    string filename;
    cout << "Hello to our simple calculator which can read expressions from any file" << endl;
    cout << "Then return the Decimal value for each expression." << endl;
    cout << "Please enter your filename: " << endl;
    cin >> filename;

    Main evaluator;
    evaluator.evaluateFromFile(filename);

    return 0;
}
