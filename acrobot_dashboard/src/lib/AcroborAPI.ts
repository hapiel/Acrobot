class AcrobotAPI {
  async isConnected() {
    try {
      await fetch('http://acrobot.local/ping');
      return true;
    } catch (e) {
      return false;
    }
  }

  async getStatus() {
    const response = await fetch('http://acrobot.local/robot-status');
    return await response.json();
  }
}

export const acrobot = new AcrobotAPI();
