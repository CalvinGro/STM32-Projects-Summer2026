

import pandas as pd
import numpy as np

ACCEL_SCALE = 8192.0
GYRO_SCALE = 131.0
GRAV_MS2 = 9.80665
ALPHA = 0.98           # percentage of filtered sample take from gyro sample (remaining .02 from accel)

df = pd.read_csv("MPU6050_Data_Modeling_P1/Data Modeling/data/imu_log.csv")

raw_accel_x = df["ax"].to_numpy(dtype=float)
raw_accel_y = df["ay"].to_numpy(dtype=float)
raw_accel_z = df["az"].to_numpy(dtype=float)

raw_gyro_x = df["gx"].to_numpy(dtype=float)
raw_gyro_y = df["gy"].to_numpy(dtype=float)
raw_gyro_z = df["gz"].to_numpy(dtype=float)

time = df["time"].to_numpy(dtype=int)
temp = df["temp"].to_numpy(dtype=float)

# converts from Gs to meters per sec squared
df["ax"] = accel_x = (raw_accel_x / ACCEL_SCALE) * GRAV_MS2
df["ay"] = accel_y = (raw_accel_y / ACCEL_SCALE) * GRAV_MS2
df["az"] = accel_z = (raw_accel_z / ACCEL_SCALE) * GRAV_MS2

df["gx"] = gyro_x = raw_gyro_x / 131
df["gy"] = gyro_y = raw_gyro_y / 131
df["gz"] = gyro_z = raw_gyro_z / 131

# implementation of Complementary Filter

# first trasfer acceleration into pitch and roll estimates.

roll_accel_estimates = np.degrees(np.atan2(accel_y, accel_z))
pitch_accel_estimates = np.degrees(np.atan2(-accel_x, np.sqrt(np.pow(accel_y, 2) + np.pow(accel_z, 2))))

pitch_estimates = np.zeros(len(df))
roll_estimates = np.zeros(len(df))

# initial estimates
prev_roll = roll_accel_estimates[0]
prev_pitch = pitch_accel_estimates[0]
prev_time = time[0] - 21   # time in ms, with 21ms being the avg time between samples

for i in range(0, len(df)):
    cur_time = ((time[i] - prev_time) / 1000) # in seconds

    gyro_roll = prev_roll + gyro_x[i] * cur_time
    gyro_pitch = prev_pitch + gyro_y[i] * cur_time

    prev_roll = roll_estimates[i] = gyro_roll * ALPHA + roll_accel_estimates[i] * (1 - ALPHA)
    prev_pitch = pitch_estimates[i] = gyro_pitch * ALPHA + pitch_accel_estimates[i] * (1 - ALPHA)
    prev_time = time[i]

df["CF_x"] = roll_estimates
df["CF_y"] = pitch_estimates

df.to_csv("MPU6050_Data_Modeling_P1/Data Modeling/data/imu_log_post_CF.csv")
