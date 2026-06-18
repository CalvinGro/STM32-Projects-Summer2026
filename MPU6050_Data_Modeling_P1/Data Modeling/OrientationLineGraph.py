import plotly.express as px
import pandas as pd


df = pd.read_csv("MPU6050_Data_Modeling_P1/Data Modeling/data/imu_log_post_CF.csv")

fig = px.line(
    df, 
    x="time", 
    y=["CF_x", "CF_y", "gx", "gy", "gz"], 
    title="Orientations of Complementary Filtered IMU Data",
    color_discrete_map={
        "CF_y": "#39913D",
        "CF_x": "#396191",
        "gx": "#91398D",
        "gy": "#916939",
        "gz": "#398D91"
    },
    labels={
        "time": "Time (ms)",
        "value": "Angle (degrees)",
        "variable": "axes"
    })

fig.show()