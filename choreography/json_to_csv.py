import csv
import json
import os

# Get the current working directory
script_directory = os.path.dirname(os.path.abspath(__file__))

# Load JSON data from AnimationCommands.json file in the same directory
json_file_path = os.path.join(script_directory, "AnimationCommands.json")
with open(json_file_path, "r") as json_file:
    data = json.load(json_file)
    
    
# Extract and store controller setup data
controller_setup_commands = data[0]["Setup"]["Controller Setup Commands"]
controller_setup_lines = controller_setup_commands.strip("\\n").split("\\n")
controller_data = {}

print(controller_setup_lines)

for line in controller_setup_lines:
    print(line)
    parts = line.split(",")
    motor = parts[1]  # Extract the label, e.g., m_arm_r
    range = [int(x) for x in parts[2:4]]  # Extract the first two numbers
    controller_data[motor] = range

# Print or save controller data as needed
print("Controller Setup Data:")
print(controller_data)

# Create a subdirectory 'csv' if it doesn't exist
csv_directory = os.path.join(script_directory, "csv")
os.makedirs(csv_directory, exist_ok=True)


def fMap(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min



# Iterate through each animation in the JSON data
for animation in data[0]["Animations"]:
    # Get animation name and commands
    animation_name = animation["Animation Name"]
    animation_commands = animation["Animation Commands"]

    # split the animation commands into multiple lines
    animation_array = animation_commands.strip("\\n").split("\\n")

    # Create a CSV file for each animation in the 'csv' subdirectory
    csv_file_path = os.path.join(csv_directory, f"{animation_name}.csv")
    with open(csv_file_path, "w", newline="") as csvfile:
        csvwriter = csv.writer(csvfile)
        
        # Write header row
        header_row = ["Command", "Motor", "Time start", "Duration", "Start pos", "Start cp X", "Start cp Y", "End pos", "End cp X", "End cp Y"]
        csvwriter.writerow(header_row)

        # Write animation commands to the CSV file
        for row in animation_array:
            row_values = row.split(",")
            motor_name = row_values[1]
            
            if motor_name in controller_data:
                range_low, range_high = controller_data[motor_name]
                row_values[4] = fMap(int(row_values[4]), 0, 8192, range_low, range_high)
                row_values[6] = fMap(int(row_values[6]), 0, 8192, range_low, range_high) - range_low
                row_values[7] = fMap(int(row_values[7]), 0, 8192, range_low, range_high)
                row_values[9] = fMap(int(row_values[9]), 0, 8192, range_low, range_high) - range_low
                
            csvwriter.writerow(row_values)

    print(f"CSV file created for animation: {csv_file_path}")
