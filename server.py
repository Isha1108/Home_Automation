from flask import Flask, request, jsonify
import pandas as pd
from datetime import datetime

app = Flask(__name__)
EXCEL_FILE = 'device_data.xlsx'

@app.route("/", methods=['GET'])
def hello():
    print(request.args.get("var"))
    return "We received value: "+str(request.args.get("var"))

@app.route('/api/data', methods=['GET'])
def insert_data():
    try:
        # Get the data from the query parameters
        timestamp = datetime.now()
        motor_status = request.args.get('MOTOR_STATE')  # Default to 'off' if not provided
        red_led = request.args.get('RED_STATE')
        yellow_led = request.args.get('YELLOW_STATE', 'off')
        green_led = request.args.get('GREEN_STATE', 'off')

        # Prepare the data
        new_data = {
            "Timestamp": timestamp,
            "Motor Status": motor_status,
            "Red LED": red_led,
            "Yellow LED": yellow_led,
            "Green LED": green_led
        }

        # Read the existing data from the Excel file
        try:
            df = pd.read_excel(EXCEL_FILE, engine="openpyxl")
        except FileNotFoundError:
            df = pd.DataFrame()

        # Append the new data
        df = df._append(new_data, ignore_index=True)
        df.to_excel(EXCEL_FILE, index=False, engine="openpyxl")

        return jsonify({"message": "Data inserted successfully", "data": new_data})
    except Exception as e:
        return jsonify({"error": str(e)}), 400

@app.route('/api/graph_data', methods=['GET'])
def get_data_for_graphs():
    try:
        # Read data from Excel file
        df = pd.read_excel(EXCEL_FILE)
        return jsonify(df.to_dict(orient='records'))
    except Exception as e:
        return jsonify({"error": str(e)}), 400

if __name__ == '__main__':
    app.run(debug=True)