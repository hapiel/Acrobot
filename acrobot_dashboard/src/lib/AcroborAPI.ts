export type LimbStatus = {
  position: number;
  velocity: number;
  torque: number;
  temperature: number;
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
      await fetch('http://acrobot.local/ping', { signal: signal.signal });
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
