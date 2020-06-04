from flask import Flask
from flask import request
import pymysql

app = Flask(__name__)

@app.errorhandler(404)
def page_not_found(error):
    print("404 error")
    return "404 error 발생"

@app.route('/main')
def main_page():
    print("main 실행")
    return "This is main page!!"

@app.route('/',methods=['GET', 'POST'])
def add_data():
    print("1구간")
    if request.method == 'POST':
        print("2구간")
        try:
            print("3구간")
            sensor_number = request.form['sensor_number']
            value = request.form['value']
            print(str(sensor_number)+' '+str(value))
            db = get_connection()
            with db.cursor() as cursor:
                cursor.execute("INSERT INTO air_quality (sensor_number, value, time) VALUES (" + sensor_number + ',' + value + ',' + "now());")
                db.commit()
        except:
            return "post error"
    elif request.method =="GET":
        print("4구간")
        try:
            sensor_number = request.args.get('sensor_number')
            value = request.args.get('value')
            db = get_connection()
            with db.cursor() as cursor:
                cursor.execute(
                    "INSERT INTO co2 (sensor_number, value, time) VALUES (" + sensor_number + ',' + value + ',' + "now());")
                db.commit()
        except:
            return "get error"

    print("5구간")
    return "success"

def get_connection():
    return pymysql.connect(host='', port=3306, user='user', password='',
                           db='fourGround', charset='utf8')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
