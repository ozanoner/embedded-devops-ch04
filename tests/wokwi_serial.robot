*** Settings ***
Library      Process
Library      OperatingSystem

*** Variables ***
${WOKWI_CMD}     wokwi-cli
${DURATION}      15s
${OUTPUT DIR}    tmp

*** Test Cases ***
Serial log should contain expected messages
    # Make sure output dir exists before writing stdout there
    Create Directory    ${OUTPUT DIR}

    ${res}=    Run Process    ${WOKWI_CMD}
    ...        shell=True    stdout=${OUTPUT DIR}/wokwi_output.txt    stderr=STDOUT
    ...        timeout=${DURATION}    on_timeout=terminate

    ${out}=    Get File    ${OUTPUT DIR}/wokwi_output.txt

   # 1) "app: Example application" appears exactly once
    Should Contain    ${out}    app: Example application
    ${ex_cnt}=    Evaluate    '''${out}'''.count('app: Example application')
    Should Be Equal As Integers    ${ex_cnt}    1


   # 2) "app: Blinky_dispatch" appears at least three times
    Should Contain    ${out}    app: Blinky_dispatch
    ${bl_cnt}=    Evaluate    '''${out}'''.count('app: Blinky_dispatch')
    ${ok}=       Evaluate    int(${bl_cnt}) >= 3
    Should Be True    ${ok}    msg=Expected at least 3 'Blinky_dispatch' messages, got ${bl_cnt}
