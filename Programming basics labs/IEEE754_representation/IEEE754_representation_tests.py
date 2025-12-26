from IEEE754_representation import *

task_6a = [-342.5, 143.125]
task_6b = [-94.4844, 304.258]
task_7a = ['C3ED3800', '42FFE000']
task_7b = ['C073EAE000000000', '407565A000000000']

print('Вариант 3', '------------------\n', 'Задание 6(A):', sep='\n')
for num in task_6a:
    print(f'{num} (float)\n{decimal_to_ieee754(num)[0]} (восьмибайтовое представление)',
          f'{decimal_to_ieee754(num)[1]} (четырехбайтовое представление)\n', sep='\n')
print('------------------', '------------------\n', 'Задание 6(Б):', sep='\n')
for num in task_6b:
    print(f'{num} (float)\n{decimal_to_ieee754(num)[0]} (восьмибайтовое представление)',
          f'{decimal_to_ieee754(num)[1]} (четырехбайтовое представление)\n', sep='\n')
print('------------------', '------------------\n', 'Задание 7(A):', sep='\n')
for num in task_7a:
    print(f'{num} = {ieee754_to_decimal(num)}')
print('------------------', '------------------\n', 'Задание 7(Б):', sep='\n')
for num in task_7b:
    print(f'{num} = {ieee754_to_decimal(num)}')
print('------------------', '------------------\n', 'Доп. Тест:', sep='\n')
print(f'{0.05} (float)\n{decimal_to_ieee754(0.05)[0]} (восьмибайтовое представление)',
      f'{decimal_to_ieee754(0.05)[1]} (четырехбайтовое представление)\n', sep='\n')

