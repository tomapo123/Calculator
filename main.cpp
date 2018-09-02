#include <QCoreApplication>
#include <exception>
#include <QStringList>
#include <QTextStream>
#include <QStack>
#include <QList>
const QString _err_msg="Error #";
const QChar op[6] = {'+', '-', '*', '/', '^', '%'};
static QTextStream cout(stdout, QIODevice::WriteOnly);

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
double qpow(double a, long long b);
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    try
    {
        if (argc != 2)
            throw BadArgument();
        QString e = a.arguments().at(1);
        QStringList a;
        bool OK = false;
        QChar o;
        for (int i = 0; i < 6; ++i)
        {
            a = e.split(op[i], QString::SkipEmptyParts);
            if (a.size() == 2 && isNumber(a.at(0)) && isNumber(a.at(1)))
            {
                OK = true;
                o = op[i];
                break;
            }
        }
        if (!OK)
            throw BadArgument();
        double res = 0;
        switch (o.toLatin1())
        {
        case '+':
            res = a.at(0).toDouble() + a.at(1).toDouble();
            break;
        case '-':
            res = a.at(0).toDouble() - a.at(1).toDouble();
            break;
        case '*':
            res = a.at(0).toDouble() * a.at(1).toDouble();
            break;
        case '/':
            if (a.at(1).toDouble() == 0.0)
                throw DividedByZero();
            res = a.at(0).toDouble() / a.at(1).toDouble();
            break;
        case '^':
            if (!isInteger(a.at(1)))
                throw InvalidNumber();
            if (((a.at(0).toDouble() == 0.0) && (a.at(1).toLongLong() == 0)) || a.at(1).toLongLong() < 0)
                throw InvalidNumber();
            res = qpow(a.at(0).toDouble(), a.at(1).toLongLong());
            break;
        case '%':
            if (!isInteger(a.at(1)) || !isInteger(a.at(0)))
                throw InvalidNumber();
            if (a.at(1).toLongLong() == 0)
                throw DividedByZero();
            res = ((a.at(0).toLongLong() % a.at(1).toLongLong()) + abs(a.at(1).toLongLong())) % abs(a.at(1).toLongLong());
        }
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
double qpow(double a, long long b)
{
    if (!b)
        return 1.0;
    double h = qpow(a, b >> 1);
    return h * h * (b & 1 ? a : 1.0);
}
