

print("THIS CODE IS SHIT AND MAKES THE DATA 30% BIGGER")
print("read the comment below to get the idea")
exit()

##
#
# This script groups repetitions of characters
#
# methode:
# encode the character and repetitions into 4bit as following:
# 0bXXYY where
# XX=number of repetitions: 0b00=1, 0b01=2, 0b10=3, 0b11=4
# YY=base: 0b00:A, 0b01:C, 0b10:G, 0b11:T
# 8.2021, peter@traunmueller.net
#
##


input_file = open("moderna_sequence_raw.fasta", "r")
characters = input_file.read()
characters.replace("\n", "")
input_file.close()

length_total = len(characters)
print(length_total)

print("compress:")
i=0
ones=0
twos=0
threes=0
fours=0
compressed_array = []
while i < length_total-1:
#while i < 20:
    if(characters[i] == "\n"):#filter the newlines
        i=i+1
        continue
    repetitions = 1
    #we count the repetitions of a character, max is 4, so we can use 2 bytes
    if(characters[i] == characters[i+1]):
        repetitions = 2
        if(characters[i] == characters[i+2]):
            repetitions = 3
            if(characters[i] == characters[i+3]):
                repetitions = 4

    if(repetitions==1):
        ones = ones + 1
    if(repetitions==2):
        twos = twos + 1
    if(repetitions==3):
        threes = threes + 1
    if(repetitions==4):
        fours = fours + 1
    #encode the character and repetitions into 4bit as following:
    #0bXXYY where
    #XX=number of repetitions: 0b00=1, 0b01=2, 0b10=3, 0b11=4
    #YY=base: 0b00:A, 0b01:C, 0b10:G, 0b11:T
    repetitions_bin = (repetitions-1) << 2
    # 2 bits per base, 00:A, 01:C, 10:G, 11:T
    if(characters[i] == "A"):
        char_bin = 0b00
    if(characters[i] == "C"):
        char_bin = 0b01
    if(characters[i] == "G"):
        char_bin = 0b10
    if(characters[i] == "T"):
        char_bin = 0b11

    coded_byte = repetitions_bin | char_bin #bitwise OR


    #print("Pos: " + str(i) + ", Repetitions: " + str(repetitions) + " for " + \
    #characters[i] + ", coded to: " +'{:04b}'.format(coded_byte))
    compressed_array.append('{:04b}'.format(coded_byte))

    i=i+repetitions

print("Encode:")
j=0
output_file = open("output.txt", "w")
while j < len(compressed_array)-1:
    upper_byte = int(compressed_array[j],2)<<4
    lower_byte = int(compressed_array[j+1],2)
    total_byte = upper_byte | lower_byte
    output_file.write(str(total_byte) + ", ")
    #print("dec: " + str(total_byte) + " ,bin:" + '{:08b}'.format(total_byte) )
    if j%32==0 and j != 0:
        output_file.write("\n")
    j = j+2
output_file.close()

print("ones: " + str(ones))
print("twos: " + str(twos))
print("threes: " + str(threes))
print("fours: " + str(fours))
