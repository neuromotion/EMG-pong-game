# The following code was modified from the tutorial by Trevor Appleton
# http://trevorappleton.blogspot.com/2014/04/writing-pong-using-python-and-pygame.html
# This requries the dependence pygame. Use pip install pygame to get this

import pygame, sys
from pygame.locals import *

# Number of frames per second
# Change this value to speed up or slow down your game
FPS = 500

#Global Variables to be used through our program

WINDOWWIDTH = 1000
WINDOWHEIGHT = 700
LINETHICKNESS = 30
PADDLESIZE = 200
PADDLEOFFSET = 50

# Set up the colours
BLACK     = (0  ,0  ,0  )
WHITE     = (255,255,255)

#Draws the arena the game will be played in.
def drawArena():
    DISPLAYSURF.fill((0,0,0))
    #Draw outline of arena
    pygame.draw.rect(DISPLAYSURF, WHITE, ((0,0),(WINDOWWIDTH,WINDOWHEIGHT)), LINETHICKNESS*2)
    #Draw centre line
    pygame.draw.line(DISPLAYSURF, WHITE, ((WINDOWWIDTH/2),0),((WINDOWWIDTH/2),WINDOWHEIGHT), (LINETHICKNESS/4))

#Draws the paddle
def drawPaddle(paddle):
    #Stops paddle moving too low
    if paddle.bottom > WINDOWHEIGHT - LINETHICKNESS:
        paddle.bottom = WINDOWHEIGHT - LINETHICKNESS
    #Stops paddle moving too high
    elif paddle.top < LINETHICKNESS:
        paddle.top = LINETHICKNESS
    #Draws paddle
    pygame.draw.rect(DISPLAYSURF, WHITE, paddle)

#draws the ball
def drawBall(ball):
    pygame.draw.rect(DISPLAYSURF, WHITE, ball)

#moves the ball returns new position
def moveBall(ball, ballDirX, ballDirY):
    ball.x += ballDirX
    ball.y += ballDirY
    return ball

#Checks for a collision with a wall, and 'bounces' ball off it.
#Returns new direction
def checkEdgeCollision(ball, ballDirX, ballDirY):
    if ball.top == (LINETHICKNESS) or ball.bottom == (WINDOWHEIGHT - LINETHICKNESS):
        ballDirY = ballDirY * -1
    if ball.left == (LINETHICKNESS) or ball.right == (WINDOWWIDTH - LINETHICKNESS):
        ballDirX = ballDirX * -1
    return ballDirX, ballDirY

#Checks is the ball has hit a paddle, and 'bounces' ball off it.
def checkHitBall(ball, paddle1, paddle2, ballDirX):
    # print("check!")
    if ballDirX == -1 and paddle1.right == ball.left and paddle1.top < ball.bottom and paddle1.bottom > ball.top:
        print(paddle1.top)
        print(ball.top)
        print(paddle1.bottom)
        print(ball.bottom)
        return -1
    elif ballDirX == 1 and paddle2.left == ball.right and paddle2.top < ball.bottom and paddle2.bottom > ball.top:
        return -1
    else: return 1

#Checks to see if a point has been scored returns new score
def checkPointScored(ball, score1, score2):
    #reset points if left wall is hit
    if ball.left == LINETHICKNESS:
        score2 += 1
    #1 point for hitting the ball
    #elif ballDirX == -1 and paddle1.right == ball.left and paddle1.top < ball.top and paddle1.bottom > ball.bottom:
        #score += 1
        #return score
    #1 points for beating the other paddle
    elif ball.right == WINDOWWIDTH - LINETHICKNESS:
        score1 += 1

    return (score1, score2)

#Artificial Intelligence of computer player
def artificialIntelligence(ball, ballDirX, paddle2):
    #If ball is moving away from paddle, center bat
    if ballDirX == -1:
        if paddle2.centery < (WINDOWHEIGHT/2):
            paddle2.y += 1
        elif paddle2.centery > (WINDOWHEIGHT/2):
            paddle2.y -= 1
    #if ball moving towards bat, track its movement.
    elif ballDirX == 1:
        if paddle2.centery < ball.centery:
            paddle2.y += 1
        else:
            paddle2.y -=1
    return paddle2

#Displays the current score on the screen
def displayScore1(score):
    resultSurf1 = BASICFONT.render('%s' %(score), True, WHITE)
    resultRect1 = resultSurf1.get_rect()
    resultRect1.topleft = (WINDOWWIDTH/2 - 75, 75)
    DISPLAYSURF.blit(resultSurf1, resultRect1)

def displayScore2(score):
    resultSurf2 = BASICFONT.render('%s' %(score), True, WHITE)
    resultRect2 = resultSurf2.get_rect()
    resultRect2.topleft = (WINDOWWIDTH/2 + 60, 75)
    DISPLAYSURF.blit(resultSurf2, resultRect2)

#Main function
def main():
    pygame.init()
    global DISPLAYSURF
    ##Font information
    global BASICFONT, BASICFONTSIZE
    BASICFONTSIZE = 20
    BASICFONT = pygame.font.Font('freesansbold.ttf', BASICFONTSIZE)

    FPSCLOCK = pygame.time.Clock()
    DISPLAYSURF = pygame.display.set_mode((WINDOWWIDTH,WINDOWHEIGHT))
    pygame.display.set_caption('Pong')

    #Initiate variable and set starting positions
    #any future changes made within rectangles
    ballX = WINDOWWIDTH/2 - LINETHICKNESS/2
    ballY = WINDOWHEIGHT/2 - LINETHICKNESS/2
    playerOnePosition = (WINDOWHEIGHT - PADDLESIZE) /2
    playerTwoPosition = (WINDOWHEIGHT - PADDLESIZE) /2
    score1 = 0
    score2 = 0
    resetScore = 0

    #Keeps track of ball direction
    ballDirX = -1 ## -1 = left 1 = right
    ballDirY = -1 ## -1 = up 1 = down

    #Creates Rectangles for ball and paddles.
    paddle1 = pygame.Rect(PADDLEOFFSET,playerOnePosition, LINETHICKNESS,PADDLESIZE)
    paddle2 = pygame.Rect(WINDOWWIDTH - PADDLEOFFSET - LINETHICKNESS, playerTwoPosition, LINETHICKNESS,PADDLESIZE)
    ball = pygame.Rect(ballX, ballY, LINETHICKNESS, LINETHICKNESS)

    # Variables for storing the key states (0 = still, 1 = Up, 2 = Down)
    player1State = 0
    player2State = 0
    player1Up = 0
    player1Down = 0
    player2Up = 0
    player2Down = 0

    #Draws the starting position of the Arena
    drawArena()
    drawPaddle(paddle1)
    drawPaddle(paddle2)
    drawBall(ball)

    pygame.mouse.set_visible(0) # make cursor invisible

    while True: #main game loop
        for event in pygame.event.get():
            if event.type == QUIT:
                pygame.quit()
                sys.exit()
            # keyboard event parsing
            elif event.type == KEYDOWN:
                if event.key == ord('a'):
                    player1Up = 1
                    player1State = 1
                elif event.key == ord('z'):
                    player1Down = 1
                    player1State = 2
                elif event.key == ord('k'):
                    player2Up = 1
                    player2State = 1
                elif event.key == ord('m'):
                    player2Down = 1
                    player2State = 2
                elif event.key == ord('r'):
                    resetScore = 1
                elif event.key == ord('='):
                   if paddle1.height < 400:
                       paddle1.inflate_ip(0, 25)
                       paddle2.inflate_ip(0, 25)
                elif event.key == ord('-'):
                   if paddle1.height > 60:
                       paddle1.inflate_ip(0, -25)
                       paddle2.inflate_ip(0, -25)
            elif event.type == KEYUP:
                if event.key == ord('a'):
                    player1Up = 0
                    if player1Down:
                        player1State = 2
                    else:
                        player1State = 0
                elif event.key == ord('z'):
                    player1Down = 0
                    if player1Up:
                        player1State = 1
                    else:
                        player1State = 0
                elif event.key == ord('k'):
                    player2Up = 0
                    if player2Down:
                        player2State = 2
                    else:
                        player2State = 0
                elif event.key == ord('m'):
                    player2Down = 0
                    if player2Up:
                        player2State = 1
                    else:
                        player2State = 0

        # Move the paddles according to the current state
        if player1State == 1:
            paddle1.y -= 1
        elif player1State == 2:
            paddle1.y += 1

        if player2State == 1:
            paddle2.y -= 1
        elif player2State == 2:
            paddle2.y += 1

        drawArena()
        drawPaddle(paddle1)
        drawPaddle(paddle2)
        drawBall(ball)

        ball = moveBall(ball, ballDirX, ballDirY)
        ballDirX, ballDirY = checkEdgeCollision(ball, ballDirX, ballDirY)
        (score1, score2) = checkPointScored(ball, score1, score2)
        if resetScore:
            score1 = 0
            score2 = 0
            resetScore = 0
        ballDirX = ballDirX * checkHitBall(ball, paddle1, paddle2, ballDirX)
        # paddle2 = artificialIntelligence (ball, ballDirX, paddle2)

        displayScore1(score1)
        displayScore2(score2)

        pygame.display.update()
        FPSCLOCK.tick(FPS)

if __name__=='__main__':
    main()
