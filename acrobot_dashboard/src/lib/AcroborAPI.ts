export type LimbStatus = {
  target: number;
  position: number;
  velocity: number;
  torque: number;
  temperature: number;
  isCalibrating: boolean;
  isOnline: boolean;
};

export type GetStatusResponse = {
  leftLeg: LimbStatus;
  rightLeg: LimbStatus;
  leftArm: LimbStatus;
  rightArm: LimbStatus;
};

class AcrobotAPI {
  async isConnected() {
    try {
      const signal = new AbortController();
      setTimeout(() => signal.abort(), 1000);
      await fetch('http://acrobot.local:3000/ping', { signal: signal.signal });
      return true;
    } catch (e) {
      return false;
    }
  }

  async getStatus(): Promise<GetStatusResponse> {
    const response = await fetch('http://acrobot.local/robot-status');
    return await response.json();
  }
}

export const acrobot = new AcrobotAPI();
