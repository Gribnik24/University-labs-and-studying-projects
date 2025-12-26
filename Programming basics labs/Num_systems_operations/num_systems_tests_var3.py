from num_systems_operations import *

task_1a = [439, 392, 900.3125, 252.12625, 908.0625]
task_1b = ['11101s2', '1000100.1s2', '653.54s8', '1FB.Bs16']

print('Вариант 3', '------------------', 'Задание 1(А):', sep='\n')
for num in task_1a:
    print(f'{num}(10) = {from_10_into_sth(num, 2)}(2)', end=' = ')
    print(f'{from_10_into_sth(num, 8)}(8)', end=' = ')
    print(f'{from_10_into_sth(num, 16)}(16)')

print('------------------', '------------------', 'Задание 1(Б):', sep='\n')
for num in task_1b:
    print(f'{num[:num.find('s')]}({num[num.find('s') + 1:]}) = '
          f'{from_sth_into_10(num[:num.find('s')], int(num[num.find('s') + 1:]))}(10)')

print('------------------', '------------------', 'Задание 2:', sep='\n')
print(f'10011010.101(2) + 1110001.101(2) = {unusual_nums_sum(10011010.101, 1110001.101, 2)}')
print(f'101011010.011(2) - 111100.1(2) = {unusual_nums_sub(101011010.011, 111100.1, 2)}')
print(f'155.6(8) + 25.1(8) = {unusual_nums_sum(155.6, 25.1, 8)}')
print(f'700.7(8) – 502.6(8) = {unusual_nums_sub(700.7, 502.6, 8)}')
print(f'160.A(16) + D2.4(16) = {unusual_nums_sum('160.A', 'D2.4', 16)}')
print(f'49.6(16) – 16.0(16) = {unusual_nums_sub('49.6', '16.0', 16)}')
print(f'11001101.1(2) ∙ 100111001.011(2) = {unusual_nums_mult('11001101.1', '100111001.011', 2)}')
print(f'432.0(8) ∙ 77.0(8) = {unusual_nums_mult('432.0', '77.0', 8)}')
print(f'F0.E(16) ∙ 1.2(16) = {unusual_nums_mult('F0.E', '1.2', 16)}')
