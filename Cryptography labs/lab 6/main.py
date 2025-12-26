import streamlit as st
from LehmannTester import generate_prime, is_prime_deterministic


def parse_input_file(file_content):
    """Парсит входной файл с параметрами"""
    lines = file_content.strip().split('\n')
    params = {}

    try:
        if len(lines) >= 2:
            params['bit_length'] = int(lines[0].strip())
            params['k_iterations'] = int(lines[1].strip())
        return params
    except Exception as e:
        st.error(f"Ошибка парсинга файла: {e}")
        return None


def main():
    st.title("Генерация простых чисел с помощью теста Леманна")
    st.write("---")

    # Инициализация session state
    if 'file_params' not in st.session_state:
        st.session_state.file_params = {}
    if 'results' not in st.session_state:
        st.session_state.results = None

    # Выбор способа ввода параметров
    input_method = st.radio(
        "Выберите способ ввода параметров:",
        ["Загрузить из файла", "Ввести вручную"],
        horizontal=True
    )

    use_file = (input_method == "Загрузить из файла")

    col1, col2 = st.columns(2)

    with col1:
        if use_file:
            st.subheader("Загрузка параметров из файла")

            uploaded_file = st.file_uploader(
                "Загрузите файл с параметрами (.txt)",
                type=["txt"],
                help="Файл должен содержать две строки: разрядность и количество итераций"
            )

            if uploaded_file is not None:
                try:
                    file_content = uploaded_file.read().decode("utf-8")
                    params = parse_input_file(file_content)

                    if params:
                        st.session_state.file_params = params
                        st.success("Файл успешно загружен!")
                        st.info(f"""
                        **Загруженные параметры:**
                        - Разрядность: {params['bit_length']} бит
                        - Итераций теста: {params['k_iterations']}
                        """)
                except Exception as e:
                    st.error(f"Ошибка чтения файла: {e}")

        else:
            st.subheader("Ручной ввод параметров")

            manual_bit_length = st.slider(
                "Разрядность числа (бит):",
                min_value=4,
                max_value=100,
                value=16,
                step=1,
                help="Количество бит в генерируемом числе"
            )

            manual_k_iterations = st.slider(
                "Количество итераций теста:",
                min_value=1,
                max_value=30,
                value=5,
                step=1,
                help="Чем больше итераций, тем точнее тест (меньше вероятность ошибки)"
            )

            st.session_state.manual_params = {
                'bit_length': manual_bit_length,
                'k_iterations': manual_k_iterations
            }

    with col2:
        st.subheader("Параметры для генерации")

        # Определяем какие параметры использовать
        if use_file and st.session_state.file_params:
            params = st.session_state.file_params
            st.info("Используются параметры из загруженного файла")
        else:
            if 'manual_params' in st.session_state:
                params = st.session_state.manual_params
                st.info("Используются параметры ручного ввода")
            else:
                params = {'bit_length': 16, 'k_iterations': 5}
                st.info("Используются параметры по умолчанию")

        # Отображаем текущие параметры
        if params:
            st.write(f"**Текущие параметры:**")
            st.write(f"- Разрядность: {params['bit_length']} бит")
            st.write(f"- Итераций теста: {params['k_iterations']}")

            # Кнопка генерации
            if st.button("Сгенерировать число", type="primary"):
                if params:
                    generate_prime_number(params)
                else:
                    st.error("Параметры не заданы!")

    # Раздел для отображения результатов
    st.write("---")
    display_results()


def generate_prime_number(params):
    """Генерация числа на основе параметров"""
    try:
        with st.spinner("Генерация числа..."):
            prime, gen_time, attempts = generate_prime(
                int(params['bit_length']),
                int(params['k_iterations'])
            )

        # Сохраняем результаты в session state
        st.session_state.results = {
            'prime': prime,
            'gen_time': gen_time,
            'attempts': attempts,
            'bit_length': params['bit_length'],
            'k_iterations': params['k_iterations']
        }

        st.success("Число сгенерировано!")

    except Exception as e:
        st.error(f"Ошибка при генерации: {str(e)}")


def display_results():
    """Отображение результатов генерации"""
    if st.session_state.results:
        results = st.session_state.results

        st.subheader("Результаты генерации:")

        # Основные результаты
        col1, col2, col3 = st.columns(3)
        with col1:
            st.metric("Разрядность", f"{results['bit_length']} бит")
        with col2:
            st.metric("Время генерации", f"{results['gen_time']:.4f} сек")
        with col3:
            st.metric("Попыток генерации", results['attempts'])

        # Детальная информация
        st.write(f"**Сгенерированное число:** {results['prime']}")
        st.write(f"**Количество итераций теста:** {results['k_iterations']}")

        # Детерминированная проверка (истинная природа числа)
        is_actually_prime = is_prime_deterministic(results['prime'])

        st.subheader("Анализ результатов:")

        col_analysis1, col_analysis2 = st.columns(2)

        with col_analysis1:
            st.write("**Мнение теста Леманна:**")
            st.success("✓ Вероятно простое")

        with col_analysis2:
            st.write("**Действительная природа числа:**")
            if is_actually_prime:
                st.success("✓ Простое")
            else:
                st.error("✗ Составное")

        # Анализ расхождений
        if not is_actually_prime:
            st.error(f"ОБНАРУЖЕНА ОШИБКА ТЕСТА!")
            st.error(f"Тест Леманна с {results['k_iterations']} итерациями принял составное число за простое")

            # Показываем делители для небольших чисел
            if results['prime'] < 100000:
                divisors = []
                for i in range(2, min(1000, results['prime'])):
                    if results['prime'] % i == 0:
                        divisors.append(i)
                        if len(divisors) >= 2:
                            break

                if divisors:
                    st.write(f"**Делители:** {', '.join(map(str, divisors))}")
                    if len(divisors) == 1:
                        st.write(f"**Разложение:** {results['prime']} = {divisors[0]} × {results['prime'] // divisors[0]}")
        else:
            st.success(f"Тест Леманна корректен с {results['k_iterations']} итерациями")

        # Кнопки для сохранения результатов
        st.subheader("Сохранение результатов:")
        col_save1, col_save2 = st.columns(2)

        with col_save1:
            prime_str = str(results['prime'])
            st.download_button(
                label="Скачать число (.txt)",
                data=prime_str,
                file_name="generated_number.txt",
                mime="text/plain"
            )

        with col_save2:
            report = f"""РЕЗУЛЬТАТЫ ГЕНЕРАЦИИ ЧИСЛА
Тест Леманна

ПАРАМЕТРЫ ГЕНЕРАЦИИ:
- Разрядность: {results['bit_length']} бит
- Итераций теста: {results['k_iterations']}

РЕЗУЛЬТАТЫ:
- Сгенерированное число: {results['prime']}
- Время генерации: {results['gen_time']:.6f} секунд
- Попыток генерации: {results['attempts']}
- Мнение теста Леманна: Вероятно простое
- Действительно простое: {'Да' if is_actually_prime else 'Нет'}
- Обнаружена ошибка: {'Да' if not is_actually_prime else 'Нет'}
"""
            st.download_button(
                label="Скачать полный отчет (.txt)",
                data=report,
                file_name="lehmann_test_report.txt",
                mime="text/plain"
            )

    else:
        st.info("Сгенерируйте число для просмотра результатов")


if __name__ == "__main__":
    main()