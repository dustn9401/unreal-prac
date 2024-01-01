import csv

csv_path = "WeaponAttributeTable.csv"

num_lvl = 3

with open(csv_path, 'w', newline='', encoding='utf-8') as file:
    csv_writer = csv.writer(file)
    
    header = ['Level'] + [i + 1 for i in range(num_lvl)]
    csv_writer.writerow(header)
    
    groups = ['Default', 'Pistol', 'Rifle']
    attribute_set_ammo = 'Ammo'
    for group in groups:
        row = [f'{group}.{attribute_set_ammo}.MagazineSize'] + \
              [30 + i*10 for i in range(num_lvl)]
        csv_writer.writerow(row)

        row = [f'{group}.{attribute_set_ammo}.MaxExtraAmmo'] + \
              [300 + i*100 for i in range(num_lvl)]
        csv_writer.writerow(row)

