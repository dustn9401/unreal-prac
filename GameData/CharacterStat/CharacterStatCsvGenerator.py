import csv

csv_path = "CharacterStatTable.csv"

num_lvl = 100

with open(csv_path, 'w', newline='', encoding='utf-8') as file:
    csv_writer = csv.writer(file)
    
    header = ['Level'] + [i + 1 for i in range(num_lvl)]
    csv_writer.writerow(header)
    
    groups = ['Default', 'Player', 'Enemy']
    attribute_set_hp = 'HP'
    attribute_set_stat = 'Stat'
    for group in groups:
        row = [f'{group}.{attribute_set_hp}.MaxHP'] + \
              [200 + i*50 for i in range(num_lvl)]
        csv_writer.writerow(row)

        row = [f'{group}.{attribute_set_stat}.Attack'] + \
              [(100 + i*50 if group == 'Player' else 40 + i*20) for i in range(num_lvl)]
        csv_writer.writerow(row)

        row = [f'{group}.{attribute_set_stat}.AttackRange'] + \
              [40 for i in range(num_lvl)]
        csv_writer.writerow(row)

        row = [f'{group}.{attribute_set_stat}.AttackSpeed'] + \
              [round(1 + i*0.05, 2) for i in range(num_lvl)]
        csv_writer.writerow(row)

        row = [f'{group}.{attribute_set_stat}.MovementSpeed'] + \
              [400 for i in range(num_lvl)]
        csv_writer.writerow(row)

        row = [f'{group}.{attribute_set_stat}.Defense'] + \
              [i*10 for i in range(num_lvl)]
        csv_writer.writerow(row)
