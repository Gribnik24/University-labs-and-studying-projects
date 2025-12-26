import random
import math
import time


def is_prime_lehmann(n, k):
    """
    Тест Леманна на простоту
    
    Args:
        n: Число для проверки
        k: Количество итераций теста (чем больше, тем точнее)
    """
    if n < 2:
        return False
    if n == 2 or n == 3:
        return True
    if n % 2 == 0:
        return False

    for i in range(k):
        a = random.randint(2, n - 2)
        exponent = (n - 1) // 2
        t = pow(a, exponent, n)

        if t != 1 and t != n - 1:
            return False

    return True


def is_prime_deterministic(n):
    """
    Детерминированная проверка на простоту
    """
    if n < 2:
        return False
    if n == 2:
        return True
    if n % 2 == 0:
        return False

    for i in range(3, int(math.sqrt(n)) + 1, 2):
        if n % i == 0:
            return False
    return True


def generate_prime(bit_length, k_iterations):
    """
    Генерация простого числа с помощью теста Леманна
    
    Args:
        bit_length: Разрядность числа в битах
        k_iterations: Количество итераций теста (влияет на точность)
    
    Returns:
        tuple: (число, время генерации, количество попыток)
    """
    attempts = 0
    start_time = time.perf_counter()

    while True:
        attempts += 1

        # Генерируем кандидата
        candidate = random.getrandbits(bit_length)
        if bit_length > 1:
            candidate |= (1 << (bit_length - 1))  # Старший бит
        candidate |= 1  # Младший бит (нечетность)

        # Проверяем тестом Леманна с заданным количеством итераций
        if is_prime_lehmann(candidate, k_iterations):
            end_time = time.perf_counter()
            generation_time = end_time - start_time
            return candidate, generation_time, attempts