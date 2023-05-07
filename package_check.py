import transformers
import torch
#check packgae version
print(transformers.__version__)
print(torch.__version__)
if transformers.__version__ != "4.24.0":
    print("suggested transformers version 4.24.0")
if torch.__version__ != "2.0.0":
    print("suggested torch version 2.0.0")
#suggested version
#transformers: 4.24.0
#torch: 2.0.0