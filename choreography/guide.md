# Guide to creating moves and sequences

## New move set
1. Duplicate a bottango file, or acrobot_empty.btngo
2. Set driver name as `acrobot_NAME` (in hardware menu)
3. create animations, with prefix `NAME_movename`
4. Save bottango file as `acrobot_NAME`


## Export moves
1. Export as JSON from bottango to choreography folder
2. Run json_to_csv.py
3. Upload the new move csv files to the acrobot
4. Optionally, make new menu entries for the move and reflash the firmware

## Create routines
1. Run template_updater.py if new moves have been created since last time
2. Copy template.xlsx
3. Create a template, and save it as `routine_NAME`
4. Export the template as .csv
5. Optionally, make new menu entries for the routine and reflash the firmware

## Bottango values
Legs 0.517 is legs down
Body 0.497 is body straight
