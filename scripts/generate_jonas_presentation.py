#!/usr/bin/env python3
"""
Generate a PowerPoint presentation for the JONAS Article Classification project.

This script creates slides for:
- Executive Summary
- Business Understanding
- Data Understanding (with distribution chart)
- Data Preparation
- Modeling
- Evaluation (with confusion matrix image and results table)
- Discussion & Limitations
- Recommendations
- Deployment Considerations
- Conclusion
- References
- Appendices (Technical specs, Evaluation table, Data gap analysis)

Images generated:
- images/discipline_distribution.png
- images/confusion_matrix.png
"""

from pathlib import Path
from typing import List, Tuple

import matplotlib.pyplot as plt
import numpy as np

from pptx import Presentation
from pptx.util import Inches, Pt
from pptx.enum.text import PP_ALIGN
from pptx.dml.color import RGBColor


WORKSPACE_DIR = Path("/workspace")
IMAGES_DIR = WORKSPACE_DIR / "images"
OUTPUT_PPTX = WORKSPACE_DIR / "JONAS_Classification_Project.pptx"


def ensure_dirs() -> None:
    IMAGES_DIR.mkdir(parents=True, exist_ok=True)


def generate_discipline_distribution_chart(classes: List[str], counts: List[int], out_path: Path) -> None:
    # Sort by count descending for readability
    sorted_pairs = sorted(zip(classes, counts), key=lambda x: x[1], reverse=True)
    sorted_classes = [c for c, _ in sorted_pairs]
    sorted_counts = [v for _, v in sorted_pairs]

    plt.figure(figsize=(10, 6))
    bars = plt.barh(sorted_classes, sorted_counts, color="#4C78A8")
    plt.xlabel("Article Count")
    plt.title("Discipline Frequency (Illustrative)")
    plt.gca().invert_yaxis()
    # Add labels
    for bar, value in zip(bars, sorted_counts):
        plt.text(bar.get_width() + 0.2, bar.get_y() + bar.get_height() / 2, str(value), va="center", fontsize=9)
    plt.tight_layout()
    plt.savefig(out_path, dpi=200)
    plt.close()


def generate_confusion_matrix_image(classes: List[str], distribution: List[int], out_path: Path) -> None:
    """Create an illustrative 14x14 confusion matrix highlighting bias toward 'Mining Engineering'."""
    num_classes = len(classes)
    mining_idx = classes.index("Mining Engineering") if "Mining Engineering" in classes else 0

    rng = np.random.default_rng(42)

    # Start with zeros
    cm = np.zeros((num_classes, num_classes), dtype=int)

    # Allocate correct predictions modestly, with Mining having higher correct predictions
    for i, true_count in enumerate(distribution):
        if i == mining_idx:
            correct = max(6, int(true_count * 0.6))  # more correct for Mining
        else:
            correct = max(0, int(true_count * 0.2))
        cm[i, i] = correct

    # Allocate remaining as misclassifications, biased toward predicting Mining
    for i, true_count in enumerate(distribution):
        remaining = true_count - cm[i, i]
        if remaining <= 0:
            continue
        # Allocate at least half of remaining to Mining predictions (except if true is Mining)
        to_mining = 0
        if i != mining_idx:
            to_mining = max(0, int(remaining * 0.6))
            cm[i, mining_idx] += to_mining
            remaining -= to_mining

        # Spread the rest randomly across other classes (excluding i and mining)
        candidates = [j for j in range(num_classes) if j not in {i, mining_idx}]
        if remaining > 0 and candidates:
            weights = rng.random(len(candidates))
            weights /= weights.sum()
            alloc = (weights * remaining).astype(int)
            # Fix rounding to ensure sum equals remaining
            while alloc.sum() < remaining:
                alloc[rng.integers(0, len(alloc))] += 1
            for idx, amount in zip(candidates, alloc):
                cm[i, idx] += int(amount)

    # Plot heatmap
    fig, ax = plt.subplots(figsize=(10, 8))
    im = ax.imshow(cm, cmap="Blues")
    ax.set_title("Confusion Matrix (Illustrative)")
    ax.set_xlabel("Predicted Label")
    ax.set_ylabel("True Label")
    ax.set_xticks(np.arange(num_classes))
    ax.set_yticks(np.arange(num_classes))
    ax.set_xticklabels(classes, rotation=45, ha="right", fontsize=7)
    ax.set_yticklabels(classes, fontsize=7)
    # Annotate a subset to avoid clutter
    for i in range(num_classes):
        for j in range(num_classes):
            val = cm[i, j]
            if val > 0 and (i == j or j == mining_idx or val >= 3):
                ax.text(j, i, str(val), ha="center", va="center", color="black", fontsize=7)
    fig.colorbar(im, ax=ax, fraction=0.046, pad=0.04)
    plt.tight_layout()
    plt.savefig(out_path, dpi=220)
    plt.close(fig)


def set_title_format(shape) -> None:
    run = shape.text_frame.paragraphs[0].runs[0]
    run.font.size = Pt(36)
    run.font.bold = True


def add_title_slide(prs: Presentation, title: str, subtitle: str) -> None:
    slide_layout = prs.slide_layouts[0]
    slide = prs.slides.add_slide(slide_layout)
    slide.shapes.title.text = title
    set_title_format(slide.shapes.title)
    subtitle_placeholder = slide.placeholders[1]
    subtitle_placeholder.text = subtitle


def add_bullet_slide(prs: Presentation, title: str, bullets: List[Tuple[str, int]]) -> None:
    """Add a slide with bullets. bullets is list of (text, level)."""
    slide_layout = prs.slide_layouts[1]
    slide = prs.slides.add_slide(slide_layout)
    slide.shapes.title.text = title
    set_title_format(slide.shapes.title)
    tf = slide.placeholders[1].text_frame
    tf.clear()
    for text, level in bullets:
        p = tf.add_paragraph()
        p.text = text
        p.level = level
        p.font.size = Pt(18)


def add_picture_slide(prs: Presentation, title: str, bullets: List[Tuple[str, int]], image_path: Path, image_width_inches: float = 6.5) -> None:
    slide_layout = prs.slide_layouts[1]
    slide = prs.slides.add_slide(slide_layout)
    slide.shapes.title.text = title
    set_title_format(slide.shapes.title)
    tf = slide.placeholders[1].text_frame
    tf.clear()
    for text, level in bullets:
        p = tf.add_paragraph()
        p.text = text
        p.level = level
        p.font.size = Pt(18)

    # Add picture to the right side
    left = Inches(6.8 - image_width_inches)  # position image to roughly align right edge
    top = Inches(2.0)
    height = None
    width = Inches(image_width_inches)
    slide.shapes.add_picture(str(image_path), left, top, width=width, height=height)


def add_table_slide(prs: Presentation, title: str, col_headers: List[str], rows: List[List[str]]) -> None:
    slide_layout = prs.slide_layouts[5]  # Title Only
    slide = prs.slides.add_slide(slide_layout)
    slide.shapes.title.text = title
    set_title_format(slide.shapes.title)

    rows_count = len(rows) + 1
    cols_count = len(col_headers)

    left = Inches(0.6)
    top = Inches(1.8)
    width = Inches(9.0)
    height = Inches(0.8 + 0.3 * rows_count)

    table = slide.shapes.add_table(rows_count, cols_count, left, top, width, height).table
    # Header
    for j, header in enumerate(col_headers):
        cell = table.cell(0, j)
        cell.text = header
        cell.text_frame.paragraphs[0].runs[0].font.bold = True
        cell.text_frame.paragraphs[0].alignment = PP_ALIGN.CENTER
    # Rows
    for i, row_vals in enumerate(rows, start=1):
        for j, val in enumerate(row_vals):
            cell = table.cell(i, j)
            cell.text = str(val)
            cell.text_frame.paragraphs[0].alignment = PP_ALIGN.CENTER


def build_presentation() -> None:
    ensure_dirs()

    # Data for illustrative visuals
    classes = [
        "Mining Engineering", "Geology", "Metallurgy", "Environmental Engineering",
        "Petroleum Engineering", "Civil Engineering", "Mechanical Engineering",
        "Electrical Engineering", "Chemical Engineering", "Computer Science",
        "Data Science", "Materials Science", "Safety Engineering", "Operations Research",
    ]
    counts = [18, 6, 4, 4, 3, 3, 2, 2, 1, 1, 1, 1, 0, 0]  # sums to 46

    dist_img = IMAGES_DIR / "discipline_distribution.png"
    cm_img = IMAGES_DIR / "confusion_matrix.png"

    generate_discipline_distribution_chart(classes, counts, dist_img)
    generate_confusion_matrix_image(classes, counts, cm_img)

    prs = Presentation()

    # Title
    add_title_slide(
        prs,
        title="Automating Classification of JONAS Articles",
        subtitle="Project Report • Dataset: 46 articles • 14 classes • September 2025",
    )

    # Executive Summary
    add_bullet_slide(
        prs,
        title="Executive Summary",
        bullets=[
            ("Problem: Manual classification is slow and inconsistent.", 0),
            ("Approach: Text preprocessing + classic ML (LR, SVM, RF).", 0),
            ("Results: Best accuracy 58.3% (SVM, Sentence-BERT embeddings).", 0),
            ("Limitations: 46 docs, 14 classes, extreme imbalance.", 0),
            ("Future: Grow data, consolidate labels, advanced models.", 0),
        ],
    )

    # 1. Business Understanding
    add_bullet_slide(
        prs,
        title="1. Business Understanding",
        bullets=[
            ("Problem: Manual classification of JONAS articles is slow/inconsistent.", 0),
            ("Objectives:", 0),
            ("Automate classification to improve speed and consistency.", 1),
            ("Target accuracy ≥ 70–85%.", 1),
            ("Reduce classification errors and improve discoverability.", 1),
            ("Success Criteria: Accuracy ≥ 70%, robust multi-class classification.", 0),
        ],
    )

    # 2. Data Understanding + chart
    add_picture_slide(
        prs,
        title="2. Data Understanding",
        bullets=[
            ("46 articles, 14 classes, class imbalance present.", 0),
            ("Data quality issues:", 0),
            ("Missing/limited keywords; small sample size.", 1),
            ("Skewed distribution toward 'Mining Engineering'.", 1),
            ("Bar chart shows discipline frequency (illustrative).", 0),
        ],
        image_path=dist_img,
        image_width_inches=5.6,
    )

    # 3. Data Preparation
    add_bullet_slide(
        prs,
        title="3. Data Preparation",
        bullets=[
            ("Preprocessing: cleaning, normalization, stopword removal.", 0),
            ("Feature engineering: Bag-of-Words and Sentence-BERT embeddings.", 0),
            ("Label encoding for discipline.", 0),
            ("Train-test split 75/25, cross-validation via LOOCV.", 0),
        ],
    )

    # 4. Modeling
    add_bullet_slide(
        prs,
        title="4. Modeling",
        bullets=[
            ("Algorithms: Logistic Regression, SVM, Random Forest.", 0),
            ("Why: LR as baseline; SVM for high-dimensional embeddings; RF as non-linear.", 0),
            ("Config: Tuned hyperparameters; used class_weight to mitigate imbalance.", 0),
            ("Training: Standardized pipeline across feature sets.", 0),
        ],
    )

    # 5. Evaluation + confusion matrix image
    add_picture_slide(
        prs,
        title="5. Evaluation",
        bullets=[
            ("Metrics: Accuracy, precision, recall, F1-score.", 0),
            ("Confusion matrix shows bias toward 'Mining Engineering'.", 0),
            ("SVM best at 58.3% accuracy; criteria not met.", 0),
        ],
        image_path=cm_img,
        image_width_inches=6.0,
    )

    # Evaluation table (illustrative numbers)
    add_table_slide(
        prs,
        title="Model Results (Illustrative)",
        col_headers=["Model", "Features", "Accuracy", "Macro F1"],
        rows=[
            ["Logistic Regression", "Bag-of-Words", "0.42", "0.35"],
            ["SVM", "Sentence-BERT", "0.583", "0.51"],
            ["Random Forest", "Bag-of-Words", "0.36", "0.30"],
        ],
    )

    # 6. Discussion & Limitations
    add_bullet_slide(
        prs,
        title="6. Discussion & Limitations",
        bullets=[
            ("Root causes of poor performance:", 0),
            ("Extreme class imbalance (dominant 'Mining Engineering').", 1),
            ("Very small dataset (n=46) vs. 14 classes.", 1),
            ("High dimensionality vs. low samples.", 1),
            ("Observed behavior: Model predicts dominant class disproportionately.", 0),
        ],
    )

    # 7. Recommendations
    add_bullet_slide(
        prs,
        title="7. Recommendations",
        bullets=[
            ("Expand dataset (≥ 200 samples per class).", 0),
            ("Merge similar disciplines to reduce complexity.", 0),
            ("Text augmentation (paraphrasing); consider synthetic balancing.", 0),
            ("Once data is sufficient: deep learning & transfer learning.", 0),
        ],
    )

    # 8. Deployment Considerations
    add_bullet_slide(
        prs,
        title="8. Deployment Considerations",
        bullets=[
            ("Prototype as CLI or simple web app for stakeholder testing.", 0),
            ("Production not recommended until dataset grows.", 0),
            ("Monitoring and human-in-the-loop oversight recommended.", 0),
        ],
    )

    # 9. Conclusion
    add_bullet_slide(
        prs,
        title="9. Conclusion",
        bullets=[
            ("Proof of concept successful; pipeline built end-to-end.", 0),
            ("Did not meet accuracy goals due to data constraints.", 0),
            ("Next: Data collection, class consolidation, advanced methods.", 0),
        ],
    )

    # 10. References
    add_bullet_slide(
        prs,
        title="10. References",
        bullets=[
            ("Libraries: scikit-learn, Sentence-BERT, pandas, numpy, matplotlib, python-pptx.", 0),
            ("Relevant academic papers on text classification and embeddings.", 0),
            ("JONAS source corpus.", 0),
        ],
    )

    # Appendices
    add_bullet_slide(
        prs,
        title="Appendix A: Technical Specs",
        bullets=[
            ("Python 3.10+; Key libs: scikit-learn, sentence-transformers, pandas, numpy.", 0),
            ("Visualization: matplotlib; Presentation: python-pptx.", 0),
            ("OS: Linux (x86_64).", 0),
        ],
    )

    add_table_slide(
        prs,
        title="Appendix B: Full Evaluation (Illustrative)",
        col_headers=["Model", "Precision", "Recall", "F1-score", "Accuracy"],
        rows=[
            ["Logistic Regression", "0.41", "0.42", "0.35", "0.42"],
            ["SVM", "0.55", "0.58", "0.51", "0.583"],
            ["Random Forest", "0.34", "0.36", "0.30", "0.36"],
        ],
    )

    add_bullet_slide(
        prs,
        title="Appendix C: Data Requirements Gap",
        bullets=[
            ("Current: 46 docs; Needed: ≥ 2,800 (14 x 200) for target.", 0),
            ("Current: Missing keywords; Needed: richer metadata/keywords.", 0),
            ("Current: Imbalance; Needed: class consolidation and sampling strategy.", 0),
        ],
    )

    # Save
    prs.save(str(OUTPUT_PPTX))


if __name__ == "__main__":
    build_presentation()

