import json

# Caminho para o ficheiro original exportado
input_file = "movies.json"
# Caminho para o novo ficheiro com array
output_file = "movies_array.json"

with open(input_file, 'r', encoding='utf-8') as f_in:
    lines = f_in.readlines()
    docs = [json.loads(line) for line in lines]

with open(output_file, 'w', encoding='utf-8') as f_out:
    json.dump(docs, f_out, indent=2)

print(f"Ficheiro convertido com sucesso para {output_file}")
