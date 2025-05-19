from fastapi import APIRouter, Depends, HTTPException
from sqlalchemy.orm import Session
from app import schemas, models, auth, database

router = APIRouter(prefix="/likes", tags=["Likes"])

@router.post("", response_model=schemas.LikeOut)
def like_photo(like: schemas.LikeCreate, db: Session = Depends(database.SessionLocal), user: models.User = Depends(auth.get_current_user)):
    existing = db.query(models.Like).filter_by(user_id=user.id, photo_id=like.photo_id).first()
    if existing:
        raise HTTPException(status_code=400, detail="Already liked")
    new_like = models.Like(user_id=user.id, photo_id=like.photo_id)
    db.add(new_like)
    db.commit()
    db.refresh(new_like)
    return new_like

@router.delete("/{like_id}")
def unlike(like_id: int, db: Session = Depends(database.SessionLocal), user: models.User = Depends(auth.get_current_user)):
    like = db.query(models.Like).filter(models.Like.id == like_id, models.Like.user_id == user.id).first()
    if not like:
        raise HTTPException(status_code=404, detail="Like not found")
    db.delete(like)
    db.commit()
    return {"detail": "Unliked"}