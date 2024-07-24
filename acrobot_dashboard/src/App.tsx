import { Limb, LimbName } from '@/components/limb/Limb';
import { Button } from '@/components/ui/button';
import { useRobotStatus } from '@/hooks/useRobotState';
import { acrobot } from '@/lib/AcroborAPI';
import { useCallback } from 'react';
import useWebSocket, { ReadyState } from 'react-use-websocket';

function App() {
  const {
    status: { rightArm, rightLeg, leftArm, leftLeg },
    setStatus
  } = useRobotStatus();
  const { sendMessage, lastMessage, readyState } = useWebSocket('ws://192.168.68.114:3000/ws', {
    shouldReconnect: () => true
  });

  const handleClickSendMessage = useCallback(() => sendMessage('hello'), [sendMessage]);

  return (
    <main className="flex min-w-full flex-col items-center justify-center pt-2">
      <div className="flex flex-col gap-2">
        <Button onClick={async () => setStatus(await acrobot.getStatus())}>Get status</Button>
        {readyState !== ReadyState.OPEN && (
          <Button disabled={readyState === ReadyState.CONNECTING} onClick={async () => sendMessage('connect')}>
            Connect socket
          </Button>
        )}
        {readyState === ReadyState.OPEN && <Button onClick={handleClickSendMessage}>Send hello</Button>}
      </div>
      <div>
        <h2>Socket status</h2>
        <p>{ReadyState[readyState]}</p>
        <h2>Last message</h2>
        <p>{JSON.stringify(lastMessage)}</p>
      </div>
      <div className="grid w-full grid-cols-2 grid-rows-2 gap-4 p-2">
        <Limb status={rightArm}>
          <LimbName>Right arm</LimbName>
        </Limb>
        <Limb status={leftArm}>
          <LimbName>Left arm</LimbName>
        </Limb>
        <Limb status={rightLeg}>
          <LimbName>Right leg</LimbName>
        </Limb>
        <Limb status={leftLeg}>
          <LimbName>Left leg</LimbName>
        </Limb>
      </div>
    </main>
  );
}

export default App;
