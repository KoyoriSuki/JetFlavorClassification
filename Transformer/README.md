# Transformer model used for the classification task

## Installation

- PyTorch v2.0.1 was used for this project. Other appropriate versions should work as well.
- This model was built upon the set transformer. The modules need to be downloaded from https://github.com/juho-lee/set_transformer. Then copy all files and folders from this directory to the path where max_regression_demo.ipynb locates.
- To run the .ipynb files, Jupyter Notebook is required. Alternatively, you can copy all the code blocks from the .ipynb file to a .py file, which should work as well.

## Files

- Files used for model training are put in ./simulationData. Generated by the provided PYTHIA script, they have to be renamed as simulationDataN.txt (the N th file) before execution.
- The trained model is saved in ./modelSave. It can be loaded for testing.
- Methods for using the files can be found in the comments in the script.


