# mRNA vaccine badge

[![](video_link_image.jpg)](https://youtu.be/r8VF4bMJ7uo "Project video")

A small badge showing a schematic of an mRNA vaccine nanoparticle and blinking the RNA sequence of the Moderna or Pfizer vaccines. (Switch between vaccines by long pressing (>1s) the button to enter the menu, short pressing to switch, and long pressing to save).

The RNA for the moderna vaccine is from is from https://github.com/NAalytics/Assemblies-of-putative-SARS-CoV2-spike-encoding-mRNA-sequences-for-vaccines-BNT-162b2-and-mRNA-1273 with an added poly-A tail of similar length to the Pfizer one (just a guess). The Pfizer sequence is from https://berthub.eu/articles/11889.doc

The sequences are stored encoded into 2 bits per nucleotide using [encode_genome.py](encode_genome.py).

Current draw is ~3mA depending on which color LEDs are active at the moment (the blue ones are a lot more efficient than the red ones). A CR2032 coin cell should last for at least 10h of continuous use (one run through the sequence takes ~10min, and it automatically shuts off afterwards).
Standby current draw is ~0.15µA, should last basically forever (years).

### Schematics:
![](https://github.com/petl/mrna_vaccine_badge/blob/main/schematics_v1.2.png)
Version 1.2 of the schematics. 

* Microcontroller can be programmed via Arduino over the Atmel ICSP programming pinout. Can be drop-in replaced with ATTiny44/ATTiny84.
* Li-ion LiPo battery has a charge/discharge/overtemperature and short circuit protection.
* Battery charging indicators on the backside. 
* Micro USB for charging. 
* Button (Momentary Switch) is hooked to an interrupt. 
* LEDs are not multi(/charlie)plexed for easier programming. 

### Components

BOM (Bill of Materials) to be added later on if needed. 

