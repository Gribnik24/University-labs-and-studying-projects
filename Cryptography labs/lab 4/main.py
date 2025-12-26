# Streamlit GUI для ЛР-4 (Вариант 2): Шифр Бофора, алфавит a..z + '_' (разделитель слов)

import streamlit as st
from beaufort_cipher import encrypt, decrypt, ALPHABET, normalize_text

st.set_page_config(page_title="Лаба 4 — Шифры подстановки и замены (Вариант 2)", layout="wide")

st.title("Лабораторная №4 — Шифры подстановки и замены")
st.caption("Вариант 2: Шифр Бофора (алфавит: a..z + '_')")

with st.sidebar:
    mode = st.radio("Режим:", ["Шифрование", "Дешифрование"])
    st.markdown(f"**Алфавит (|X|={len(ALPHABET)}):** `{ALPHABET}`")
    st.info("Пробелы/переводы строк автоматически заменяются на '_' (разделитель слов).")


def _load_text(label, key):
    up = st.file_uploader(label, type=["txt"], key=key)
    if up:
        return up.read().decode("utf-8")
    return ""


def _load_key(label, key):
    up = st.file_uploader(label, type=["txt"], key=key)
    if up:
        return up.read().decode("utf-8").strip()
    return ""


def page_encrypt():
    st.header("Шифрование (Beaufort)")
    c1, c2 = st.columns(2)
    with c1:
        src = _load_text("Файл с исходным текстом (.txt):", "plain_file") or \
              st.text_area("…или введите текст:", "attack at dawn", height=150)
        key = _load_key("Файл с ключом (.txt):", "key_file") or \
              st.text_input("…или введите ключ:", "lemon")
        st.caption("В процессе нормализации: пробелы → '_'.")
    with c2:
        if st.button("Зашифровать", type="primary"):
            try:
                ct = encrypt(src, key)
                st.success("Готово! Текст зашифрован.")
                st.text_area("Шифртекст:", ct, height=200)
                st.download_button("Скачать шифртекст (TXT)", data=ct, file_name="ciphertext.txt", mime="text/plain")
                st.download_button("Скачать нормализованный исходный текст (TXT)",
                                   data=normalize_text(src), file_name="normalized_plain.txt", mime="text/plain")
            except Exception as e:
                st.error(str(e))


def page_decrypt():
    st.header("Дешифрование (Beaufort)")
    c1, c2 = st.columns(2)
    with c1:
        ct = _load_text("Файл с шифртекстом (.txt):", "cipher_file") or \
             st.text_area("…или введите шифртекст:", "", height=150)
        key = _load_key("Файл с ключом (.txt):", "key_file_dec") or \
              st.text_input("…или введите ключ:", "lemon")
    with c2:
        if st.button("Дешифровать", type="primary"):
            try:
                pt = decrypt(ct, key)
                st.success("Готово! Текст расшифрован.")
                st.text_area("Расшифрованный текст:", pt, height=200)
                st.download_button("Скачать расшифрованный текст (TXT)", data=pt, file_name="plaintext.txt", mime="text/plain")
            except Exception as e:
                st.error(str(e))


if mode == "Шифрование":
    page_encrypt()
else:
    page_decrypt()
