import csv

csv_path = "CharacterStatTable.csv"

num_lvl = 100

with open(csv_path, 'w', newline='', encoding='utf-8') as file:
    csv_writer = csv.writer(file)
    
    header = ['Level'] + [i + 1 for i in range(num_lvl)]
    csv_writer.writerow(header)
    
    groups = ['Default', 'Player', 'Enemy']
    attribute_set = 'CharacterStat'
    for group in groups:
        row = [f'{group}.{attribute_set}.MaxHP'] + \
              [200 + i*50 for i in range(num_lvl)]
        csv_writer.writerow(row)

        row = [f'{group}.{attribute_set}.Attack'] + \
              [40 + i*20 for i in range(num_lvl)]
        csv_writer.writerow(row)

        row = [f'{group}.{attribute_set}.AttackRange'] + \
              [40 for i in range(num_lvl)]
        csv_writer.writerow(row)

        row = [f'{group}.{attribute_set}.AttackSpeed'] + \
              [round(1 + i*0.05, 2) for i in range(num_lvl)]
        csv_writer.writerow(row)

        row = [f'{group}.{attribute_set}.MovementSpeed'] + \
              [400 for i in range(num_lvl)]
        csv_writer.writerow(row)
