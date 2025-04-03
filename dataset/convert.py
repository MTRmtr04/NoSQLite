import json

# Caminho para o ficheiro original exportado
input_file = str(input("Name of the file you want to convert: "))
# Caminho para o novo ficheiro com array
output_file = str(input("Name of the file where you want to write the output: "))

with open(input_file, 'r', encoding='utf-8') as f_in:
    lines = f_in.readlines()
    docs = [json.loads(line) for line in lines]

with open(output_file, 'w', encoding='utf-8') as f_out:
    json.dump(docs, f_out, indent=2)

print(f"Ficheiro convertido com sucesso para {output_file}")
