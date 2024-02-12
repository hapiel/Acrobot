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
controller_name = data[0]["Controller Name"]
controller_setup_commands = data[0]["Setup"]["Controller Setup Commands"]
controller_setup_lines = controller_setup_commands.strip("\\n").split("\\n")
controller_data = {}


for line in controller_setup_lines:
    parts = line.split(",")
    motor = parts[1]  # Extract the label, e.g., m_arm_r
    range = [int(x) for x in parts[2:4]]  # Extract the first two numbers
    controller_data[motor] = range

print(f"Controller Setup Data: {controller_data}")

# Create a subdirectory 'csv' if it doesn't exist
csv_directory = os.path.join(script_directory, "moves")
os.makedirs(csv_directory, exist_ok=True)


def fMap(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min


# Iterate through each animation in the JSON data
for animation in data[0]["Animations"]:
    
    # Get animation name and commands
    animation_name = animation["Animation Name"]
    animation_commands = animation["Animation Commands"]
    if animation_commands == "":
        print(f"Skipped, because no commands: {animation_name}")
        continue

    # split the animation commands into multiple lines
    animation_array = animation_commands.strip("\\n").split("\\n")
    
    
    # Create a CSV file for each animation in the 'csv' subdirectory
    csv_file_path = os.path.join(csv_directory, f"{animation_name}.csv")
    with open(csv_file_path, "w", newline="") as csvfile:
        csvwriter = csv.writer(csvfile)
        
        # Write header row
        header_row = ["Command", "Motor", "Time start", "Duration", "Start pos", "Start cp X", "Start cp Y", "End pos", "End cp X", "End cp Y"]
        csvwriter.writerow(header_row)
        
        # get the duration of the animation
        duration = 0
        for row in animation_array:
            row_values = row.split(",")
            start_time = int(row_values[2])
            length = int(row_values[3])
            if start_time + length > duration:
                duration = start_time + length
        
        # Write animation name 
        name_row = ["Animation Name:", animation_name, "Controller name:", controller_name, "Duration:", duration, "", "", "",""]
        csvwriter.writerow(name_row)
        
        # get start positions for each motor, 999.0 is default.
        
        first_arm_l_pos = 999.0
        first_arm_r_pos = 999.0
        first_leg_l_pos = 999.0
        first_leg_r_pos = 999.0
        
        # search until the first m_arm_l is found
        for row in animation_array:
            row_values = row.split(",")
            motor_name = row_values[1]
            if motor_name in controller_data and motor_name == "m_arm_l":
                range_low, range_high = controller_data[motor_name]
                first_arm_l_pos = fMap(int(row_values[4]), 0, 8192, range_low, range_high)
                break
        
        # search until the first m_arm_r is found
        for row in animation_array:
            row_values = row.split(",")
            motor_name = row_values[1]
            if motor_name in controller_data and motor_name == "m_arm_r":
                range_low, range_high = controller_data[motor_name]
                first_arm_r_pos = fMap(int(row_values[4]), 0, 8192, range_low, range_high)
                break
        
        for row in animation_array:
            row_values = row.split(",")
            motor_name = row_values[1]
            if motor_name in controller_data and motor_name == "m_leg_l":
                range_low, range_high = controller_data[motor_name]
                first_leg_l_pos = fMap(int(row_values[4]), 0, 8192, range_low, range_high)
                break
        
        for row in animation_array:
            row_values = row.split(",")
            motor_name = row_values[1]
            if motor_name in controller_data and motor_name == "m_leg_r":
                range_low, range_high = controller_data[motor_name]
                first_leg_r_pos = fMap(int(row_values[4]), 0, 8192, range_low, range_high)
                break
            

        
        # same format as other rows
        start_pos_row = ["Start positions", "ArmL ArmR legL legR", first_arm_l_pos, first_arm_r_pos, first_leg_l_pos, first_leg_r_pos, 0.0, 0.0, 0.0, 0.0]
        
        csvwriter.writerow(start_pos_row)

        # Write animation commands to the CSV file
        for row in animation_array:
            row_values = row.split(",")
            if row_values[0] != "sC":
                continue
            
            motor_name = row_values[1]
            
            if motor_name in controller_data:
                range_low, range_high = controller_data[motor_name]
                row_values[4] = fMap(int(row_values[4]), 0, 8192, range_low, range_high)
                row_values[6] = fMap(int(row_values[6]), 0, 8192, range_low, range_high) - range_low
                row_values[7] = fMap(int(row_values[7]), 0, 8192, range_low, range_high)
                row_values[9] = fMap(int(row_values[9]), 0, 8192, range_low, range_high) - range_low
                
            csvwriter.writerow(row_values)
    
    # Remove the last newline char
    with open(csv_file_path, "r") as csvfile:
        lines = csvfile.readlines()
    
    if lines:
        lines[-1] = lines[-1].rstrip("\n")
        
        
    with open(csv_file_path, "w", newline="") as csvfile:
        csvfile.writelines(lines)
    
    print(f"CSV file created for animation: {csv_file_path}")
