import streamlit as st
import pandas as pd


from ShannonCoder_Class import ShannonCoder
from metrics import calculate_metrics


def create_distribution(alphabet_file, odds_file):
    """
    Создает распределение на основе принятых файлов
    :param alphabet_file: файл с символами
    :param odds_file: файл с вероятностями
    :return: массив с парами (символ, вероятность)
    """
    distribution_array = []
    with open(alphabet_file) as file:
        alphabet = file.read().split()
    with open(odds_file) as file:
        odds = file.read().split()

    for symbol, odd in zip(alphabet, odds):
        distribution_array.append((symbol, eval(odd)))

    return distribution_array


def main():

    # Словарь с распределениями
    predefined_distributions = {
        "Равномерное": create_distribution('alphabet.txt', 'uniform odds.txt'),
        "P1(A)": create_distribution('alphabet.txt', 'P1(A) odds.txt'),
        "P2(A)": create_distribution('alphabet.txt', 'P2(A) odds.txt')
    }

    st.set_page_config(page_title="Кодер Шеннона", layout="wide")
    st.title("Кодер/Декодер Шеннона")
    st.markdown("Веб-приложение для кодирования и декодирования сообщений с использованием алгоритма Шеннона")

    # Сайдбар с навигацией
    st.sidebar.title("Навигация")
    app_mode = st.sidebar.radio("Выберите режим:",
                                ["Кодирование", "Декодирование"])

    if app_mode == "Кодирование":
        render_encoding_section(predefined_distributions)

    elif app_mode == "Декодирование":
        render_decoding_section(predefined_distributions)


def render_encoding_section(predefined_distributions):
    """
    Вкладка с настройками кодирования текста
    :param predefined_distributions: массив с параметрами распределения
    """
    st.header("Кодирование текста")

    col1, col2 = st.columns(2)

    with col1:
        st.subheader("Настройки кодирования")

        # Выбор распределения
        distribution_choice = st.selectbox(
            "Выберите распределение вероятностей:",
            list(predefined_distributions.keys())
        )

        # Показываем выбранное распределение в виде таблицы
        symbols_probs = predefined_distributions[distribution_choice]
        df = pd.DataFrame(symbols_probs, columns=["Символ", "Вероятность"])
        st.dataframe(df, use_container_width=True)

        # Сохраняем для использования
        st.session_state.current_distribution = symbols_probs

        # Ввод текста для кодирования
        input_text = st.text_area(
            "Текст для кодирования (символы через пробел):",
            value="2 20 0 200 00 _",
            height=100
        )

    with col2:
        st.subheader("Результаты кодирования")

        if st.button("Выполнить кодирование", type="primary"):
            try:
                symbols_probs = predefined_distributions[distribution_choice]

                if not symbols_probs:
                    st.error("Ошибка: неверный формат алфавита")
                    return

                # Строим коды
                coder = ShannonCoder()
                codes = coder.build_codes(symbols_probs)

                # Кодируем текст
                encoded_text = coder.encode(input_text)

                # Вычисляем метрики
                prob_dict = dict(symbols_probs)
                metrics = calculate_metrics(codes, prob_dict)

                # Сохраняем в сессии для скачивания
                st.session_state.encoding_results = {
                    'original_text': input_text,
                    'encoded_text': encoded_text,
                    'codes': codes,
                    'metrics': metrics
                }

                # Отображаем результаты
                display_encoding_results(input_text, encoded_text, codes, metrics)

            except Exception as e:
                st.error(f"Ошибка при кодировании: {str(e)}")


def render_decoding_section(predefined_distributions):
    """
    Вкладка с настройками декодирования текста
    :param predefined_distributions: массив с параметрами распределения
    """
    st.header("Декодирование текста")

    col1, col2 = st.columns(2)

    with col1:
        st.subheader("Настройки декодирования")

        distribution_choice = st.selectbox(
            "Выберите распределение вероятностей:",
            list(predefined_distributions.keys())
        )

        symbols_probs = predefined_distributions[distribution_choice]
        df = pd.DataFrame(symbols_probs, columns=["Символ", "Вероятность"])
        st.dataframe(df, use_container_width=True)
        st.session_state.decode_distribution = symbols_probs

        encoded_text = st.text_area(
            "Бинарная последовательность для декодирования:",
            value="000001100010101110",
            height=100,
            placeholder="Введите последовательность из 0 и 1..."
        )

    with col2:
        st.subheader("Результаты декодирования")

        if st.button("Выполнить декодирование", type="primary"):
            try:
                # Получаем распределение
                symbols_probs = predefined_distributions[distribution_choice]

                if not symbols_probs:
                    st.error("Ошибка: неверный формат алфавита")
                    return
                for symbol in encoded_text:
                    if symbol not in ('0', '1'):
                        st.error("Ошибка: бинарная последовательность задана неверно")
                        return

                # Декодируем
                coder = ShannonCoder()
                coder.build_codes(symbols_probs)
                decoded_text = coder.decode(encoded_text)

                # Сохраняем результаты
                st.session_state.decoding_results = {
                    'encoded_text': encoded_text,
                    'decoded_text': decoded_text
                }

                # Отображаем результаты
                display_decoding_results(encoded_text, decoded_text)

            except Exception as e:
                st.error(f"Ошибка при декодировании: {str(e)}")


def display_encoding_results(original_text, encoded_text, codes, metrics):
    """
    Отображает результаты кодирования
    :param original_text: исходный текст
    :param encoded_text: закодированный текст-результат
    :param codes: коды символов
    :param metrics: словарь метрик
    """
    st.success("Кодирование выполнено успешно!")

    # Таблица кодовых слов
    st.subheader("Кодовые слова")
    codes_df = pd.DataFrame([
        {"Символ": symbol, "Код": code, "Длина": len(code)}
        for symbol, code in codes.items()
    ])
    st.dataframe(codes_df, use_container_width=True)

    # Метрики
    st.subheader("Метрики кодирования")
    col1, col2 = st.columns(2)

    with col1:
        st.metric("Средняя длина кодового слова", f"{metrics['average_length']:.4f}")
        st.metric("Энтропия источника", f"{metrics['entropy']:.4f}")
        st.metric("Избыточность", f"{metrics['redundancy']:.4f}")

    with col2:
        kraft_status = "Выполнено" if metrics['kraft_inequality'] else "Не выполнено"
        st.metric("Неравенство Крафта", kraft_status)
        st.metric("Сумма Крафта", f"{metrics['kraft_sum']:.4f}")

    # Исходный и закодированный текст
    st.subheader("Тексты")
    col1, col2 = st.columns(2)

    with col1:
        st.text_area("Исходный текст:", original_text, height=100)

    with col2:
        st.text_area("Закодированная последовательность:", encoded_text, height=100)
        st.write(f"**Длина:** {len(encoded_text)} бит")

    # Сохранение в TXT файл
    results = {
        'original_text': original_text,
        'encoded_text': encoded_text,
        'codes': codes,
        'metrics': metrics
    }
    txt_content = save_results_to_txt(results, "encoding")

    st.download_button(
        label="Скачать результаты в TXT",
        data=txt_content,
        file_name="encoding_results.txt",
        mime="text/plain"
    )


def display_decoding_results(encoded_text, decoded_text):
    """
    Отображает результаты декодирования
    :param encoded_text: исходный закодированный текст
    :param decoded_text: декодированный текст-результат
    """
    st.success("Декодирование выполнено успешно!")

    col1, col2 = st.columns(2)

    with col1:
        st.text_area("Закодированная последовательность:", encoded_text, height=100)

    with col2:
        st.text_area("Декодированный текст:", decoded_text, height=100)

    # Сохранение в TXT файл
    results = {
        'encoded_text': encoded_text,
        'decoded_text': decoded_text
    }
    txt_content = save_results_to_txt(results, "decoding")

    st.download_button(
        label="Скачать результаты в TXT",
        data=txt_content,
        file_name="decoding_results.txt",
        mime="text/plain"
    )


def save_results_to_txt(results, mode="encoding"):
    """
    Сохраняет результаты кодирования или декодирования в TXT файл
    :param results: словарь с результатами
    :param mode: режим - "encoding" или "decoding"
    :return: TXT файл с результатами
    """

    if mode == "encoding":
        content = f"РЕЗУЛЬТАТЫ КОДИРОВАНИЯ\n"

        content += f"Исходный текст:\n{results['original_text']}\n\n"
        content += f"Закодированная последовательность:\n{results['encoded_text']}\n\n"
        content += f"Длина закодированной последовательности: {len(results['encoded_text'])} бит\n\n"

        content += "Кодовые слова:\n"
        for symbol, code in results['codes'].items():
            content += f"  '{symbol}' -> {code} (длина: {len(code)})\n"
        content += "\n"

        content += "Метрики кодирования:\n"
        metrics = results['metrics']
        content += f"  Средняя длина кодового слова: {metrics['average_length']:.4f}\n"
        content += f"  Энтропия источника: {metrics['entropy']:.4f}\n"
        content += f"  Избыточность: {metrics['redundancy']:.4f}\n"
        content += f"  Неравенство Крафта: {'Выполнено' if metrics['kraft_inequality'] else 'Не выполнено'}\n"
        content += f"  Сумма Крафта: {metrics['kraft_sum']:.4f}\n"

    elif mode == "decoding":
        content = f"РЕЗУЛЬТАТЫ ДЕКОДИРОВАНИЯ\n"

        content += f"Закодированная последовательность:\n{results['encoded_text']}\n\n"
        content += f"Декодированный текст:\n{results['decoded_text']}\n\n"
        content += f"Длина закодированной последовательности: {len(results['encoded_text'])} бит\n"

    return content


if __name__ == "__main__":
    main()
#%%
