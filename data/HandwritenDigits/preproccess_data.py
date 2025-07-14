# Read the input file and write to a new file without commas
with open('Original/optdigits.tra', 'r') as infile, open('Original/train.txt', 'w') as outfile:
    for line in infile:
        no_commas = line.replace(',', ' ')
        outfile.write(no_commas)