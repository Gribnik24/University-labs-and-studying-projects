import math


class ShannonCoder:
    def __init__(self):
        self.codes = {}

    def build_codes(self, symbols_probs):
        """
        Построение кодов по методу Шеннона
            :param symbols_probs: массив распределния
        """
        sorted_syms = sorted(symbols_probs, key=lambda x: x[1], reverse=True)
        cumulative = 0
        codes = {}

        for symbol, prob in sorted_syms:
            if prob > 0:
                # Длина кода = ceil(-log2(p))
                code_length = math.ceil(-math.log2(prob))

                # Преобразуем кумулятивную вероятность в двоичную дробь
                binary_fraction = cumulative
                code = ""

                for i in range(code_length):
                    binary_fraction *= 2
                    if binary_fraction >= 1:
                        code += "1"
                        binary_fraction -= 1
                    else:
                        code += "0"

                codes[symbol] = code
                # Увеличиваем кумулятивную вероятность после вычисления кода
                cumulative += prob

        self.codes = codes
        return codes

    def encode(self, text):
        """Упрощенное кодирование без разделителя
        :param text: строка для кодирования
        """
        symbols = text.split()
        return "".join(self.codes[symbol] for symbol in symbols)

    def decode(self, binary_string):
        """Декодирование по префиксному коду
        :param binary_string: строка для декодирования"""
        reverse_codes = {v: k for k, v in self.codes.items()}
        decoded_symbols = []
        current_code = ""

        for bit in binary_string:
            current_code += bit
            if current_code in reverse_codes:
                decoded_symbols.append(reverse_codes[current_code])
                current_code = ""

        return " ".join(decoded_symbols)