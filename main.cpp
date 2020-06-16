// Сreated by Alyona Dorodnyaya, IU8-25
// Examination papers № 3

#include <exception>
#include <future>
#include <iostream>
#include <ostream>
#include <random>
#include <thread>

class perc_exc : public std::invalid_argument {
private:
  double bad_percent;

public:
  explicit perc_exc(double p)
      : bad_percent(p), std::invalid_argument("Error."
                                              " Bad "
                                              "annual"
                                              " "
                                              "interest rate: " +
                                              std::to_string(p) + "%") {}
};

class Bank_account {
private:
  size_t sum_;
  double percent_;

public:
  Bank_account() : sum_(0), percent_(1) {}
  Bank_account(size_t s, double p) : sum_(s), percent_(p) {
    if (percent_ <= 0 || percent_ >= 100)
      throw perc_exc(percent_);
  }

  [[nodiscard]] size_t sum() const { return sum_; }
  [[nodiscard]] double percent() const { return percent_; }

  virtual void print() const {
    std::cout << "Sum: " << sum_ << "$, annual interest rate = " << percent_
              << "%.\n";
  }

  void print_1() const {
    std::cout << "Sum: " << sum_ << "$, annual interest rate = " << percent_
              << "%.\n";
  }

  friend std::istream &operator>>(std::istream &stream, Bank_account &acc) {
    stream >> acc.sum_ >> acc.percent_;
    if (acc.percent_ <= 0 || acc.percent_ >= 100)
      throw perc_exc(acc.percent_);
    return stream;
  }

  friend std::ostream &operator<<(std::ostream &stream,
                                  const Bank_account &acc) {
    stream << "Sum: " << acc.sum_
           << "$, annual interest rate = " << acc.percent_ << "%.\n";
    return stream;
  }
};

class Bank_account_overdraft : public Bank_account {
private:
  size_t over;

public:
  Bank_account_overdraft(size_t s, double p, size_t o)
      : Bank_account(s, p), over(o) {}

  void print() const override {
    std::cout << "Sum: " << sum() << "$, annual interest rate = " << percent()
              << "%, overdraft = " << over << "$.\n";
  }

  void print_1() const {
    std::cout << "Sum: " << sum() << "$, annual interest rate = " << percent()
              << "%.\n";
  }

  bool operator<(const Bank_account_overdraft &lhs) const {
    return sum() < lhs.sum();
  }
};

std::vector<Bank_account_overdraft> generate(size_t n, bool y) {
  std::random_device rd;
  std::uniform_int_distribution<size_t> dist1(0, 3000);
  std::uniform_int_distribution<double> dist2(1, 99);
  std::vector<Bank_account_overdraft> accounts;
  for (size_t i = 0; i < n; ++i) {
    size_t a = dist1(rd);
    double b = dist2(rd);
    size_t c = dist1(rd);
    Bank_account_overdraft d{a, b, c};
    if (y)
      d.print();
    accounts.push_back(d);
  }
  return accounts;
}

std::mutex MyMutex;
void Func(const std::vector<Bank_account_overdraft> &acc) {
  MyMutex.lock();
  for (const auto &a : acc) {
    a.print();
  }
  MyMutex.unlock();
}

size_t Func_2(const std::vector<Bank_account_overdraft> &acc) {
  size_t sum = 0;
  for (const auto &a : acc) {
    sum += a.sum();
  }
  return sum;
}

int main() {
  try {
    // конструктор и функция print
    Bank_account acc{600, 12}, acc1;
    acc.print();

    // перегрузка операторов >> и <<
    std::cin >> acc >> acc1;
    std::cout << acc << acc1;

    //исключения
    Bank_account acc2{12000, 150};

  } catch (perc_exc &d) {
    std::cout << d.what() << std::endl;
  }

  // наследование. Класс Bank_account_overdraft
  Bank_account_overdraft acc_o{12000, 15, 15000};
  acc_o.print();

  // Примеры статического и динамического полиморфизма
  Bank_account *acc = &acc_o;
  // статический - по типу указателя
  acc->print_1();
  // динамический - по типу объекта
  acc->print();

  // std::vector, std::sort for class Bank_account_overdraft
  size_t n, m;
  std::cout << "Размер вектора для сортировки" << std::endl;
  std::cin >> n;
  std::vector<Bank_account_overdraft> accounts = generate(n, true); //
  // генерируем c выводом
  // вектор счетов
  std::sort(accounts.begin(), accounts.end());
  for (size_t i = 0; i < n; ++i) {
    std::cout << i + 1 << ") ";
    accounts.at(i).print();
  }

  // Многопоточность
  std::cout << "Размер второго вектора" << std::endl;
  std::cin >> m;
  std::cout << "Accounts_1" << std::endl;
  std::thread a1(Func, std::cref(accounts));
  a1.join();
  std::cout << "Accounts_2" << std::endl;
  std::vector<Bank_account_overdraft> accounts2 = generate(m, false); //
  // генерируем без вывода
  std::thread a2(Func, std::cref(accounts2));
  a2.join();

  // Использование std::async и std::future c вторым массивом
  std::future<size_t> funcRes(std::async(Func_2, std::cref(accounts2)));
  size_t sum = funcRes.get();
  std::cout << "Общая сумма: " << sum << "$" << std::endl;

  return 0;
}