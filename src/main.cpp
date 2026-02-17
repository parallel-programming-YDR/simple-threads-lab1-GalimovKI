#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

void CountPointsInArea(std::atomic<int> &points_in_circle, int tries, int seed, int r)
{
  std::mt19937                           rand_engine(seed);
  std::uniform_real_distribution<double> dist(-r, r);

  for (int i = 0; i < tries; ++i)
  {
    double x = dist(rand_engine);
    double y = dist(rand_engine);
    if (x * x + y * y <= r * r)
    {
      points_in_circle.fetch_add(1, std::memory_order_relaxed);
    }
  }
}

double MonteCarloMethod(int tries, int threads, int r, int seed)
{
  unsigned hw = std::thread::hardware_concurrency();
  if (hw == 0)
    hw = 1;
  threads = std::min(threads, (int)hw);

  int tries_per_thread = tries / threads;
  int tries_remains    = tries % threads;

  std::atomic<int>         points_in_circle{0};
  std::vector<std::thread> workers;
  workers.reserve(threads);

  for (int i = 0; i < threads; ++i)
  {
    int local_tries = tries_per_thread + (i == 0 ? tries_remains : 0);
    workers.emplace_back(CountPointsInArea, std::ref(points_in_circle), local_tries, seed + i, r);
  }

  for (auto &w : workers)
    w.join();

  return 4.0 * r * r * points_in_circle.load() / tries;
}

int main(int argc, char **argv)
{
  // Обработка аргументов командной строки
  if (argc < 2 || argc > 3)
  {
    std::cerr << "Invalid number of elements\n";
    return 1;
  }

  int tries = 0;
  int seed  = 0;

  try
  {
    tries = std::stoi(argv[1]);
    if (tries <= 0)
    {
      std::cerr << "Tries must be > 0\n";
      return 1;
    }
  }
  catch (...)
  {
    std::cerr << "Invalid tries\n";
    return 1;
  }

  if (argc == 3)
  {
    try
    {
      seed = std::stoi(argv[2]);
      if (seed < 0)
      {
        std::cerr << "Seed must be >= 0\n";
        return 1;
      }
    }
    catch (...)
    {
      std::cerr << "Invalid seed\n";
      return 1;
    }
  }

  // Обработка стандартного потока ввода
  int threads = 0;
  int r       = 0;
  while (std::cin >> r >> threads)
  {
    if (threads <= 0 || r <= 0)
    {
      std::cerr << "R > 0 and Threads > 0\n";
      return 1;
    }

    auto   start = std::chrono::steady_clock::now();
    double area  = MonteCarloMethod(tries, threads, r, seed);
    auto   end   = std::chrono::steady_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << std::fixed << std::setprecision(3) << duration.count() << "  " << area << "\n";
  }

  if (std::cin.fail())
  {
    std::cerr << "Invalid pairs\n";
    return 1;
  }

  return 0;
}
