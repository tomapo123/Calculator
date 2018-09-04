#include <QCoreApplication>
#include <exception>
#include <QStringList>
#include <QTextStream>
#include <QStack>
#include <QList>
const QString _err_msg="Error #";
const QChar op[8] = {'+', '-', '*', '/', '^', '%', '(', ')'};
static QTextStream cout(stdout, QIODevice::WriteOnly);

struct Element
{
    double val;
    QChar op;
    Element() :val(0.0), op(0) {}
    Element(double v, QChar o) :val(v), op(o) {}
};

class BadArgument :public std::exception
{
public:
    const char *what() const noexcept;
private:
    const unsigned int ID = 0;
    const QString Name = "Bad argument";
};
class DividedByZero :public std::exception
{
public:
    const char *what() const noexcept;
private:
    const unsigned int ID = 1;
    const QString Name = "Divided by 0";
};
class InvalidNumber :public std::exception
{
public:
    const char *what() const noexcept;
private:
    const unsigned int ID = 2;
    const QString Name = "Using invalid number";
};
class UnknownError :public std::exception
{
public:
    const char *what() const noexcept;
private:
    const unsigned int ID = 3;
    const QString Name = "Unknown error happened, please contact administrator for help";
};

QList<Element> infixExpressionSeperator(const QString &rhs);
QList<Element> infixToSuffix(const QList<Element> &rhs);
double calculateSuffixExpression(const QList<Element> &rhs);
double qpow(double a, long long b);
double calc2(double a, double b, const QChar &o);

inline bool isInteger(const QString &rhs)
{
    return QString::number(rhs.toLongLong()) == rhs;
}
inline bool isInteger(double rhs)
{
    return double((int)rhs) == rhs;
}
inline bool isNumber(const QString &rhs)
{
    return QString::number(rhs.toDouble(), 'f').toDouble() == rhs.toDouble() || isInteger(rhs);
}
inline bool isOp(const QChar &rhs)
{
    for (int i = 0; i < 8; ++i)
        if (rhs == op[i])
            return true;
    return false;
}
inline double calculateInfixExpression(const QString &rhs)
{
    return calculateSuffixExpression(infixToSuffix(infixExpressionSeperator(rhs)));
}
inline unsigned int priority(const QChar &rhs)
{
    switch (rhs.toLatin1())
    {
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
    case '%':
        return 2;
    case '^':
        return 3;
    case '(':
        return 0;
    }
    return -1;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    try
    {
        if (argc != 2)
            throw BadArgument();
        double res = calculateInfixExpression(a.arguments().at(1));
        if (isInteger(res))
        {
            long long r = (int)res;
            cout << r << endl;
        }
        else
        {
            cout.setRealNumberNotation(QTextStream::FixedNotation);
            cout << res << endl;
        }
    }
    catch (const std::exception &e)
    {
        cout << e.what() << endl;
        a.exit(EXIT_FAILURE);
    }
    return 0;
}
const char *BadArgument::what() const noexcept
{
    return (_err_msg + QString::number(ID) + ": " + Name).toLatin1().data();
}
const char *DividedByZero::what() const noexcept
{
    return (_err_msg + QString::number(ID) + ": " + Name).toLatin1().data();
}
const char *InvalidNumber::what() const noexcept
{
    return (_err_msg + QString::number(ID) + ": " + Name).toLatin1().data();
}
const char *UnknownError::what() const noexcept
{
    return (_err_msg + QString::number(ID) + ": " + Name).toLatin1().data();
}
double qpow(double a, long long b)
{
    if (!b)
        return 1.0;
    double h = qpow(a, b >> 1);
    return h * h * (b & 1 ? a : 1.0);
}
QList<Element> infixExpressionSeperator(const QString &rhs)
{
    QList<Element> ret;
    QString now_val;
    int bracketsDelta = 0;
    bool haveDot = false;
    for (int i = 0; i < rhs.size(); ++i)
    {
        if (isOp(rhs[i]))
        {
            if (now_val.isEmpty() && rhs[i] != '(' && (i >= 1 ? rhs[i - 1] != ')' : 1))
            {
                throw BadArgument();
            }
            if (rhs[i].toLatin1() == '(')
            {
                ++bracketsDelta;
                ret << Element(0.0, rhs[i]);
                continue;
            }
            if (rhs[i].toLatin1() == ')')
            {
                --bracketsDelta;
            }
            if (!now_val.isEmpty() || (i >= 1 ? rhs[i - 1] != ')' : 0) || rhs[i] != '(')
                ret << Element(now_val.toDouble(), QChar(0)) << Element(0.0, rhs[i]);
            now_val.clear();
            haveDot = false;
        }
        else if (rhs[i].isDigit())
            now_val += rhs[i];
        else if (rhs[i].toLatin1() == '.' && !haveDot)
        {
            now_val += QChar('.');
            haveDot = true;
        }
        else
            throw BadArgument();
        cout << i;
    }
    ret << Element(now_val.toDouble(), QChar(0));
    if (bracketsDelta)
        throw BadArgument();
    for (int i = 0; i < ret.size(); ++i)
        cout << ret.at(i).op << ret.at(i).val << endl;
    return ret;
}
QList<Element> infixToSuffix(const QList<Element> &rhs)
{
    QStack<QChar> os;
    QList<Element> ret;
    for (int i = 0; i < rhs.size(); ++i)
        if (!rhs.at(i).op.toLatin1())
            ret << Element(rhs.at(i).val, QChar(0));
        else if (rhs.at(i).op.toLatin1() == '(')
            os.push(rhs.at(i).op);
        else if (rhs.at(i).op.toLatin1() == ')')
        {
            while (os.top().toLatin1() != '(')
                ret << Element(0.0, os.pop());
            os.pop();
        }
        else if (os.isEmpty())
            os.push(rhs.at(i).op);
        else
        {
            while ((priority(os.top()) >= priority(rhs.at(i).op)) || os.isEmpty())
                ret << Element(0.0, os.pop());
            os.push(rhs.at(i).op);
        }
    while (!os.isEmpty())
        ret << Element(0.0, os.pop());
    return ret;
}
double calculateSuffixExpression(const QList<Element> &rhs)
{
    QStack<double> ns;
    for (int i = 0; i < rhs.size(); ++i)
        if (!rhs.at(i).op.toLatin1())
            ns.push(rhs.at(i).val);
        else
        {
            double l = ns.pop();
            double r = ns.pop();
            ns.push(calc2(r, l, rhs.at(i).op));
        }
    while (!ns.isEmpty())
        cout << ns.pop() << endl;

    if (ns.size() != 1)
        throw UnknownError();
    return ns.top();
}
double calc2(double a, double b, const QChar &o)
{
    double res = 0;
    switch (o.toLatin1())
    {
        case '+':
            res = a + b;
            break;
        case '-':
            res = a - b;
            break;
        case '*':
            res = a * b;
            break;
        case '/':
            if (b == 0.0)
                throw DividedByZero();
            res = a / b;
            break;
        case '^':
            if (!isInteger(b))
                throw InvalidNumber();
            if (((a == 0.0) && (b == 0.0)) || b < 0.0)
                throw InvalidNumber();
            res = qpow(a, (int)b);
            break;
        case '%':
            if (!isInteger(b) || !isInteger(a))
                throw InvalidNumber();
            if (b == 0.0)
                throw DividedByZero();
            long long aa = static_cast<long long>(a);
            long long bb = static_cast<long long>(b);
            res = ((aa % bb) + abs(bb)) % abs(bb);
        }
    return res;
}
